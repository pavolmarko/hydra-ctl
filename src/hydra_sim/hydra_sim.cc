
#include "hydra_lib/hydra_radio.h"
#include "hydra_lib/hydra_msgs.h"


const int LONG_TIMEOUT = 60*60*1000;

uint8_t GetCurPosResp(int cur_pos, int min_pos, int max_pos);

int main(int argc, char* argv[]) {
  const int POS_MIN = 0;
  const int POS_MAX = 20;

  int cur_pos = 10;

  // TODO: In reality, this is not 1 :-)
  auto hydra_radio = hydra::HydraRadio::Create(1, 0);
  if (hydra_radio == nullptr) {
    return 1;
  }
  while (true) {
    auto res = hydra_radio->Recv(LONG_TIMEOUT);
    if (res.has_value()) {
      printf("Received %d!\n", res.value());
    } else {
      printf("Recv failed %d!\n", (int)res.error());
      return 1;
    }
    switch (res.value()) {
      case hydra::REQ_HELLO:
        hydra_radio->Send(0, hydra::RSP_HELLO);
        break;
      case hydra::REQ_GET_POS:
        hydra_radio->Send(0, GetCurPosResp(cur_pos, POS_MIN, POS_MAX));
        break;
      case hydra::REQ_OPEN_START:
        if (cur_pos == POS_MIN) {
          hydra_radio->Send(0, GetCurPosResp(cur_pos, POS_MIN, POS_MAX));
        }
        else {
          hydra_radio->Send(0, hydra::RSP_OPENING);
        }
        break;
      case hydra::REQ_OPEN_CONTINUE:
        if (cur_pos == POS_MIN) {
          // For some reason, it gets sent twice.
          hydra_radio->Send(0, GetCurPosResp(cur_pos, POS_MIN, POS_MAX));
          hydra_radio->Send(0, GetCurPosResp(cur_pos, POS_MIN, POS_MAX));
        }
        else {
          --cur_pos;
          hydra_radio->Send(0, hydra::RSP_OPENING);
        }
        break;
      default:
        printf("Dunno %d!\n", res.value());
        break;
    }
  }
  
  return 0;

}

uint8_t GetCurPosResp(int cur_pos, int min_pos, int max_pos) {
  if (cur_pos == min_pos) {
    return hydra::RSP_POS_FULLY_OPEN;
  }
  if (cur_pos == max_pos) {
    return hydra::RSP_POS_FULLY_CLOSED;
  }
  return hydra::RSP_POS_INBETWEEN;
}
