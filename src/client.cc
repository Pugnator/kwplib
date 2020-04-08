#include "message.hpp"
#include "client.hpp"
#include <math.h>

namespace KWP2000
{
const kwp_service *kwpClient::find_service_id(const service_mnemonic &name)
{
  for (auto i = 0; service_ids[i].request; ++i)
  {
    if (service_ids[i].short_name == name)
    {
      return &service_ids[i];
    }
  }
  return nullptr;
}
} // namespace KWP2000
