#include "message.hpp"
#include "client.hpp"
#include <exception>

namespace KWP2000
{

kwpClient::kwpClient(uint8_t commport, uint32_t baudrate)
{
  open_port(commport, baudrate);
  portn = commport;
}

kwpClient::~kwpClient()
{
}

void kwpClient::open_port(uint8_t commport, uint32_t baudrate)
{
}

std::unique_ptr<kwp_message> kwpClient::read_response(kwp_message &request)
{
  std::unique_ptr<kwp_message> response(new kwp_message);
  return response;
}

void kwpClient::send_message(kwp_message &message)
{
}

} // namespace KWP2000