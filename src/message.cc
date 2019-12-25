#include <message.hpp>

namespace KWP2000
{
const kwp_service service_ids[] =
    {
        {.request = 0x7F, .reply = 0x7F, .sae_name = "negativeReply", .short_name = KWP_NR},
        {.request = 0x81, .reply = 0xC1, .sae_name = "startCommunication", .short_name = KWP_STC},
        {.request = 0x82, .reply = 0xC2, .sae_name = "stopCommunication", .short_name = KWP_SPC},
        {.request = 0x10, .reply = 0x50, .sae_name = "startDiagnosticSession", .short_name = KWP_STDS},
        {.request = 0x20, .reply = 0x60, .sae_name = "stopDiagnosticSession", .short_name = KWP_SPDS},
        {.request = 0x11, .reply = 0x51, .sae_name = "ecuReset", .short_name = KWP_ER},
        {.request = 0x14, .reply = 0x54, .sae_name = "clearDiagnosticInformation", .short_name = KWP_CDI},
        {.request = 0x18, .reply = 0x58, .sae_name = "readDiagnosticTroubleCodesByStatus", .short_name = KWP_RDTCBS},
        {.request = 0x1A, .reply = 0x5A, .sae_name = "readEcuIdentification", .short_name = KWP_REI},
        {.request = 0x21, .reply = 0x61, .sae_name = "readDataByLocalIdentifier", .short_name = KWP_RDBLI},
        {.request = 0x23, .reply = 0x63, .sae_name = "readMemoryByAddress", .short_name = KWP_RMBA},
        {.request = 0x30, .reply = 0x70, .sae_name = "inputOutputControlByLocalIdentifier", .short_name = KWP_IOCBLI},
        {.request = 0x3B, .reply = 0x7B, .sae_name = "writeDataByLocalIdentifier", .short_name = KWP_WDBLI},
        {.request = 0x3E, .reply = 0x7E, .sae_name = "testerPresent", .short_name = KWP_TP},
        {0, 0}};

const kwp_reply kwp_reply_table[] =
    {
        {.reply = 0x10, .sae_name = "generalReject", .short_name = GR},
        {.reply = 0x11, .sae_name = "serviceNotSupported", .short_name = SNS},
        {.reply = 0x12, .sae_name = "subFunctionNotSupported-invalidFormat", .short_name = SFNS_IF},
        {.reply = 0x21, .sae_name = "busy-RepeatRequest", .short_name = B_RR},
        {.reply = 0x31, .sae_name = "requestOutOfRange", .short_name = ROOR},
        {.reply = 0x72, .sae_name = "transferAborted", .short_name = GR},
        {.reply = 0x77, .sae_name = "blockTransferDataChecksumError", .short_name = BTDCE},
        {0, 0}};

const kwp_service *kwp_message::get_service()
{
  uint8_t id = data[header.type - 1];
  printf("Searching for 0x%X\r\n", id);  
  for (auto i = 0; service_ids[i].request; ++i)
  {
    if (service_ids[i].request == id || service_ids[i].reply == id)
    {
      printf("found %s\r\n", service_ids[i].sae_name);
      return &service_ids[i];
    }
  }
  return nullptr;
}

void kwp_message::add_service_id(const kwp_service &id)
{
  data[header.type] = id.request;
  length++;
}

void kwp_message::add_param(const uint8_t &param)
{
  printf("Param: 0x%X\r\n", param);
  data[header.type + length++] = param;  
}

void kwp_message::add_payload(uint8_t *payload, uint8_t size)
{
  memcpy(data + header.type - 1, payload, size);
}

void kwp_message::print()
{
  printf("Message header: 0x%X 0x%X 0x%X\r\n", header.format, header.target, header.source);
  printf("Message of type 0x%X, length 0x%X [CRC: 0x%X]\r\n",
         header.type,
         length,
         checksum);
}

void kwp_message::update_format_byte()
{
  header.format |= 1 << 7;
  header.format |= length & 0b00111111;
}

bool kwp_service::bad() const
{
  return short_name == KWP_NR;
}

void kwp_message::update_crc()
{
  checksum = header.get_sum();
  for (auto i = 0; i < length; ++i)
  {
    checksum += data[header.type + i];
  }
  data[header.type + length] = checksum;
}

void kwp_message::update_header()
{
  update_format_byte();
  data[0] = header.format;
  data[1] = header.target;
  data[2] = header.source;
  if (header.type == HEADER_LONG)
  {
    data[3] = length;
  }
}

uint8_t kwp_header::get_sum()
{
  return format + target + source + length;
}

} // namespace KWP2000