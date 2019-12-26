#include <message.hpp>
#include <client.hpp>
#include <algorithm>

namespace KWP2000
{

std::unique_ptr<kwp_message> kwpClient::process_command(const service_mnemonic &cmd, const std::initializer_list<uint8_t> &params)
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

bool kwpClient::stop_diagnostic_session()
{
  auto resp = process_command(KWP_SPDS);
  if (!resp)
  {
    return false;
  }
  return true;
}

ECU_identification_table *kwpClient::readEcuIdentification(const identificationOption &options)
{
  auto resp = process_command(KWP_REI, {options});
  if (!resp)
  {
    return nullptr;
  }
  printf("Size: 0x%X [%u]\r\n", resp->length, resp->length);
  for (auto i = 1; i < resp->length; i++)
  {
    printf("%c", resp->data[resp->header.type + i]);
  }

  std::memcpy(ECU_ID.data, &resp->data[resp->header.type + 1], sizeof(ECU_identification_table));

  return &ECU_ID;
}

bool kwpClient::clearDiagnosticInformation()
{
  return true;
}
bool kwpClient::ecuReset()
{
  auto resp = process_command(KWP_ER, {0x1});
  if (!resp)
  {
    return false;
  }
  return true;
}
bool kwpClient::inputOutputControlByLocalIdentifier()
{
  return true;
}
bool kwpClient::readDataByLocalIdentifier()
{
  return true;
}
bool kwpClient::readDiagnosticTroubleCodesByStatus()
{
  return true;
}
bool kwpClient::readMemoryByAddress()
{
  return true;
}
bool kwpClient::testerPresent(bool responseRequired)
{
  auto resp = process_command(KWP_TP, {static_cast<uint8_t>(responseRequired ? 0x1 : 0x2)});
  if (!resp)
  {
    return false;
  }
  return true;
}
bool kwpClient::writeDataByLocalIdentifier()
{
  return true;
}
} // namespace KWP2000