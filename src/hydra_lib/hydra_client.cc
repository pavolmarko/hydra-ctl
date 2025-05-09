#include "hydra_lib/hydra_client.h"

namespace hydra {

const int TIMEOUT_MILLIS = 1000;

HydraClient::HydraClient(HydraRadio* radio, uint16_t remote_addr) : radio_(radio), remote_addr_(remote_addr) {
}

HydraClient::~HydraClient(){
}


std::expected<void, Error> HydraClient::Connect(){
  radio_->ResetRecvCounter();
  radio_->Send(remote_addr_, REQ_HELLO);
  auto resp = radio_->Recv(TIMEOUT_MILLIS);
  if (!resp.has_value()) {
    return std::unexpected(Error::SERVER_HELLO_TIMEOUT);
  }
  if (resp.value() != RSP_HELLO) {
    return std::unexpected(Error::SERVER_HELLO_INVALID);
  }
  auto state = SendReq(REQ_GET_STATE);
  if (!resp.has_value()) {
    return std::unexpected(state.error());
  }
  // TODO remember state
  printf("got state %x\n", state.value());
  return {};
}

std::expected<uint8_t, Error> HydraClient::SendReq(uint8_t req){
  std::expected<uint8_t, Error> last_res;
  for (int i = 0; i < 3; ++i ) { 
    radio_->Send(remote_addr_, req);
    last_res = radio_->Recv(TIMEOUT_MILLIS);
    if (last_res.has_value()) {
      return last_res;
    }
    radio_->DecrementSendCounter();
  }
  return last_res;
}


} // namespace hydra
