#ifndef __HYDRA_CTL_HYDRA_CLIENT__H__
#define __HYDRA_CTL_HYDRA_CLIENT__H__


#include <stdint.h>

#include <memory>
#include <expected>
#include <optional>

#include "hydra_lib/hydra_radio.h"
#include "hydra_lib/hydra_msgs.h"

namespace hydra {

class HydraClient {
 public:
  HydraClient(HydraRadio* radio, uint16_t remote_addr);
  ~HydraClient();

  std::expected<void, Error> Connect();
  std::expected<uint8_t, Error> SendReq(uint8_t req);

 private:
  HydraRadio* const radio_;
  const uint16_t remote_addr_;
};

} // hydra

#endif  // __HYDRA_CTL_HYDRA_CLIENT__H__

