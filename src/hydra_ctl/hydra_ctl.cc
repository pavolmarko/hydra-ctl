
#include <chrono>
#include <thread>


#include "hydra_lib/hydra_client.h"
#include "hydra_lib/hydra_radio.h"
#include "hydra_lib/hydra_msgs.h"



int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Need one arg: open or close\n");
    exit(1);
  }
  const std::string what = argv[1];

  uint8_t start_cmd;
  uint8_t continue_cmd;
  uint8_t in_progress_status;
  uint8_t done_status;
  if (what == "open") {
    start_cmd = hydra::REQ_OPEN_START;
    continue_cmd = hydra::REQ_OPEN_CONTINUE;
    in_progress_status = hydra::RSP_OPENING;
    done_status = hydra::RSP_POS_FULLY_OPEN;
  } else if (what == "close") {
    start_cmd = hydra::REQ_CLOSE_START;
    continue_cmd = hydra::REQ_CLOSE_CONTINUE;
    in_progress_status = hydra::RSP_CLOSING;
    done_status = hydra::RSP_POS_FULLY_CLOSED;
  } else {
    printf("need open or close - got %s\n", what.c_str());
    exit(1);
  }

  auto hydra_radio = hydra::HydraRadio::Create(0, 0);
  if (hydra_radio == nullptr) {
    return 1;
  }

  // TODO: This should be 0 instead :-)
  const int rem_addr = 0;
  hydra::HydraClient client(hydra_radio.get(), rem_addr);

  client.Connect();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  auto res = client.SendReq(start_cmd);
  if (!res.has_value()) {
    printf("ERR on REQ_OPEN_START");
    return 1;
  }
  if (res.value() != in_progress_status) {
    printf("unexpected rsp %x on start cmd", res.value());
    return 1;
  }

  while (true) {
    // 100ms seems to be the default timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto res = client.SendReq(continue_cmd);
    if (!res.has_value()) {
      printf("ERR on continue req");
      return 1;
    }
    if (res.value() == in_progress_status) {
      printf("Still opening\n");
      // Good.
    } else if (res.value() == done_status) {
      // It sends another status
      hydra_radio->Recv(400);
      printf("Done!\n");
      return 0;
    } else {
      printf("unexpected rsp %x on continue cmd", res.value());
      return 1;
    }
  }
  
  return 0;

}
