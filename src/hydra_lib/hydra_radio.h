#ifndef __HYDRA_CTL_HYDRA_RADIO_H__
#define __HYDRA_CTL_HYDRA_RADIO_H__

#include <stdint.h>

#include <memory>
#include <expected>
#include <optional>

#include "hydra_lib/hydra_msgs.h"

namespace hydra {

class HydraRadio {
 public:
  ~HydraRadio();

  static std::unique_ptr<HydraRadio> Create(uint16_t my_addr, uint8_t send_packet_counter_start);

  void Send(uint16_t remote_addr, uint8_t command);
  std::expected<uint8_t, Error> Recv(int timeout_millis);

  void ResetRecvCounter();
  void DecrementSendCounter();

  static const int COUNTER_START_CTL = 0x00;
  static const int COUNTER_START_SIM = 0x60;

 private:
  HydraRadio(int socket, uint8_t send_packet_counter_start);

  void IncrementSendPacketCounter();
  void IncrementRecvPacketCounter();

  const int socket_;
  uint8_t send_packet_counter_;
  std::optional<uint8_t> recv_packet_counter_;
};

} // hydra

#endif  // __HYDRA_CTL_HYDRA_RADIO_H__
