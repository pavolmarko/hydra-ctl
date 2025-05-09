#include "hydra_lib/hydra_radio.h"

#include <expected>
#include <memory>

#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <limits.h>

#include <netlink/netlink.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include "../netlink_hdr/nl802154.h"

namespace hydra {

const int IEEE802154_ADDR_LEN = 8;
const int IEEE802154_ADDR_SHORT = 0x2;

const unsigned int HYDRA_PACKET_LEN = 3;

struct ieee802154_addr_sa {
  int addr_type;
  uint16_t pan_id;
  union {
    uint8_t hwaddr[IEEE802154_ADDR_LEN];
    uint16_t short_addr;
  };
};

struct sockaddr_ieee802154 {
  sa_family_t family;
  struct ieee802154_addr_sa addr;
};

HydraRadio::HydraRadio(int socket, uint8_t send_packet_counter_start) : socket_(socket) { 
  send_packet_counter_ = send_packet_counter_start;
}

HydraRadio::~HydraRadio() {
  close(socket_);
}

// static
std::unique_ptr<HydraRadio> HydraRadio::Create(uint16_t my_addr, uint8_t send_packet_counter_start) {
  sockaddr_ieee802154 src;
  src.family = AF_IEEE802154;
  // TODO: read from interface instead (NL802154_ATTR_PAN_ID)
  src.addr.pan_id = 0x72;
  src.addr.addr_type = IEEE802154_ADDR_SHORT;
  // TODO: read from interface instead (NL802154_ATTR_SHORT_ADDR)
  src.addr.short_addr = my_addr;

  int sd = socket(PF_IEEE802154, SOCK_DGRAM, 0);
  if (sd < 0) {
    perror("socket");
    return nullptr;
  }

  /* Bind socket on this side */
  int ret = bind(sd, (sockaddr *)&src, sizeof(src));
  if (ret) {
    perror("bind");
    close(sd);
    return nullptr;
  }

  return std::unique_ptr<HydraRadio>(new HydraRadio(sd, send_packet_counter_start));
}

void HydraRadio::Send(uint16_t remote_addr, uint8_t command) {
  IncrementSendPacketCounter();

  sockaddr_ieee802154 dst;
  dst.family = AF_IEEE802154;
  dst.addr.pan_id = 0x72;
  dst.addr.addr_type = IEEE802154_ADDR_SHORT;
  dst.addr.short_addr = remote_addr;

  uint8_t buf[HYDRA_PACKET_LEN];
  buf[0] = send_packet_counter_;
  buf[1] = 0;
  buf[2] = command;

  printf("Send %x %x %x\n", buf[0], buf[1], buf[2]);
  int ret = sendto(socket_, buf, HYDRA_PACKET_LEN, 0, (sockaddr *)&dst, sizeof(dst));
  if (ret < 0) {
    perror("sendto");
  }
}

std::expected<uint8_t, Error> HydraRadio::Recv(int timeout_millis) {
  timeval timeout;

  timeout.tv_sec = timeout_millis / 1000;
  timeout.tv_usec = (timeout_millis % 1000) * 1000;

  int ret = setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if (ret < 0) {
    perror("setsockopt receive timeout");
    return std::unexpected(Error::CONF_RECV_TIMEOUT);
  }

  printf("Waiting for response\n");
  for (int i = 0; i < 8; ++i) {
    uint8_t buf[HYDRA_PACKET_LEN];
    ret = recv(socket_, buf, HYDRA_PACKET_LEN, 0);
    if (ret < 0) {
      perror("recv");
      // TODO: check something specific?
      // This means it timed out or couldn't recv
      return std::unexpected(Error::RECV_TIMEOUT);
    }
    printf("Got %x %x %x\n", buf[0], buf[1], buf[2]);

    if (recv_packet_counter_.has_value()) {
      if (recv_packet_counter_.value() != buf[0]) {
        // Ignore this one - maybe the correct packet will still arrive.
        continue;
      }
    } else {
      // Expect RSP_HELLO if no counter was seen yet
      if (buf[2] != RSP_HELLO) {
        printf("Found RSP_HELLO - now recv_packet_counter is %d\n", *recv_packet_counter_);
        continue;
      }
      recv_packet_counter_ = buf[0];
    }
    IncrementRecvPacketCounter();
    return buf[2];
  }
  return std::unexpected(Error::RECV_TIMEOUT);
}

void HydraRadio::ResetRecvCounter() {
  recv_packet_counter_.reset();
}

void HydraRadio::IncrementSendPacketCounter() {
  ++send_packet_counter_;
  // 0x00 (happens after overflow) should be skipped according to packet dumps.
  if (send_packet_counter_ == 0x00) {
    send_packet_counter_ = 0x01;
  }
}

void HydraRadio::IncrementRecvPacketCounter() {
  // CHECK(recv_packet_counter_.has_value());
  (*recv_packet_counter_)++;
  // 0x00 (happens after overflow) should be skipped according to packet dumps.
  if (*recv_packet_counter_ == 0x00) {
    *recv_packet_counter_ = 0x01;
  }
  printf("now expecting recv packet counter %x\n", *recv_packet_counter_);
}

void HydraRadio::DecrementSendCounter() {
  //todo handle 0x00
  --send_packet_counter_;
}

} // hydra
