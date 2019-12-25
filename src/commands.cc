#include <message.hpp>
#include <tester.hpp>

namespace KWP2000
{

std::unique_ptr<kwp_message> kwpClient::process_command(const service_id &cmd, const std::initializer_list<uint8_t> &params)
{
  puts("\r\n====================\r\n");
  auto rq = find_service_id(cmd);
  if (!rq)
  {
    return nullptr;
  }
  kwp_message message;
  message.add_service_id(*rq);
  for (auto p : params)
  {
    message.add_param(p);
  }
  send_message(message);
  auto reply = read_response(message);
  if (!reply)
  {
    return std::unique_ptr<kwp_message>();
  }
  auto service = reply->get_service();
  if (!service || service->bad() || service->short_name != cmd)
  {
    return std::unique_ptr<kwp_message>();
  }
  return reply;
}

bool kwpClient::start_communication()
{
  auto resp = process_command(KWP_STC);
  if (!resp)
  {
    return false;
  }
  uint16_t key = (resp->data[resp->header.type] << 8) | resp->data[resp->header.type + 1];
  if (KWP_PHY_ADR == key)
  {
    //
  }
  return true;
}

bool kwpClient::stop_communication()
{
  auto resp = process_command(KWP_SPC);
  if (!resp)
  {
    return false;
  }
  return true;
}

bool kwpClient::start_diagnostic_session()
{
  auto resp = process_command(KWP_STDS, {0x81, 0x26});
  if (!resp)
  {
    return false;
  }
  return true;
}

bool kwpClient::read_ECUid()
{
  auto resp = process_command(KWP_REI, {0x80});
  if (!resp)
  {
    return false;
  }
  printf("Size: 0x%X [%u]\r\n", resp->length, resp->length);
  for (auto i = 0; i < resp->length; i++)
  {
    printf("%.2X ", resp->data[resp->header.type + i]);
  }
  return true;
}
} // namespace KWP2000