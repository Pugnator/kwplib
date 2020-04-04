#include <message.hpp>
#include <client.hpp>
#include <dtc.hpp>
#include <algorithm>

namespace KWP2000
{

std::unique_ptr<kwp_message> kwpClient::process_command(const service_mnemonic &cmd, const std::initializer_list<uint8_t> &params)
{
  DEBUG_LOG("\r\n====================\r\n");
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
    return nullptr;
  }
  auto service = reply->get_service();
  if (!service || service->bad() || service->short_name != cmd)
  {
    return nullptr;
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

bool kwpClient::start_diagnostic_session(const uint8_t baudrate)
{
  auto resp = process_command(KWP_STDS, {0x81, baudrate});
  if (!resp)
  {
    return false;
  }
  //check if we have to increase the baudrate
  if(baudrate == NORMAL_BAUDRATE)
  {
    return true;
  }
  
  uint16_t baud = baudrate == HIGH_BAUDRATE ? 38400 : 57600;
  #ifdef WIN32
  close_port(porth);
  porth = open_port(portn, baud);
  #else
  #endif
  
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

ECU_identification_table *kwpClient::read_ECU_identification(const identificationOption &options)
{
  auto resp = process_command(KWP_REI, {options});
  if (!resp)
  {
    return nullptr;
  }
  DEBUG_LOG("Size: 0x%X [%u]\r\n", resp->length, resp->length);
  for (auto i = 1; i < resp->length; i++)
  {
    DEBUG_LOG("%c", resp->data[resp->header.type + i]);
  }

  std::memcpy(ECU_ID.data, &resp->data[resp->header.type + 1], sizeof(ECU_identification_table));

  return &ECU_ID;
}

bool kwpClient::clear_diagnostic_information(bool clear_all)
{
  auto resp = process_command(KWP_CDI, {static_cast<uint8_t>(clear_all ? 0xFF, 0x00 : 0x00, 0x00)});
  if (!resp)
  {
    return false;
  }

  return true;
}
bool kwpClient::ECU_reset()
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
std::unique_ptr<RLI_ASS_tab> kwpClient::read_rli_ass()
{
  auto resp = process_command(KWP_RDBLI, {RLI_ASS});
  if (!resp)
  {
    return std::unique_ptr<RLI_ASS_tab>{};
  }
  std::unique_ptr<RLI_ASS_tab> rlitab(new RLI_ASS_tab);
  uint8_t *payload = &resp->data[resp->header.type + 1];
  std::memcpy(rlitab->tab, payload, sizeof(rlitab->tab));
  for (auto i = 0; i < 36; i++)
  {
    DEBUG_LOG("%02X ", rlitab->tab[i]);
  }
  DEBUG_LOG("");
  return rlitab;
}

bool kwpClient::read_DTC_by_status()
{
  auto resp = process_command(KWP_RDTCBS, {0x00, 0x00, 0x00});
  if (!resp)
  {
    return false;
  }
  uint8_t *payload = &resp->data[resp->header.type];
  uint8_t dtc_num = payload[0];
  DEBUG_LOG("DTC number: %u\r\n", dtc_num);
  for (auto i = 1; i < resp->length - 1; i += 3)
  {
    DEBUG_LOG("DTC: P%04X\r\n", (payload[i] << 8) | (payload[i + 1] & 0xFF));
  }
  return true;
}
bool kwpClient::readMemoryByAddress()
{
  return true;
}
bool kwpClient::tester_present(bool response_required)
{
  auto resp = process_command(KWP_TP, {static_cast<uint8_t>(response_required ? 0x1 : 0x2)});
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