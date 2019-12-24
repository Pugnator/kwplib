#include <message.hpp>

namespace KWP2000
{
const kwp_identificator kwp_table[] =
    {
        {.request = 0x7F, .reply = 0x7F, .inter_name = "negativeReply", .short_name = KWP_NR},
        {.request = 0x81, .reply = 0xC1, .inter_name = "startCommunication", .short_name = KWP_STC},
        {.request = 0x82, .reply = 0xC2, .inter_name = "stopCommunication", .short_name = KWP_SPC},
        {.request = 0x10, .reply = 0x50, .inter_name = "startDiagnosticSession", .short_name = KWP_STDS},
        {.request = 0x20, .reply = 0x60, .inter_name = "stopDiagnosticSession", .short_name = KWP_SPDS},
        {.request = 0x11, .reply = 0x51, .inter_name = "ecuReset", .short_name = KWP_ER},
        {.request = 0x14, .reply = 0x54, .inter_name = "clearDiagnosticInformation", .short_name = KWP_CDI},
        {.request = 0x18, .reply = 0x58, .inter_name = "readDiagnosticTroubleCodesByStatus", .short_name = KWP_RDTCBS},
        {.request = 0x1A, .reply = 0x5A, .inter_name = "readEcuIdentification", .short_name = KWP_REI},
        {.request = 0x21, .reply = 0x61, .inter_name = "readDataByLocalIdentifier", .short_name = KWP_RDBLI},
        {.request = 0x23, .reply = 0x63, .inter_name = "readMemoryByAddress", .short_name = KWP_RMBA},
        {.request = 0x30, .reply = 0x70, .inter_name = "inputOutputControlByLocalIdentifier", .short_name = KWP_IOCBLI},
        {.request = 0x3B, .reply = 0x7B, .inter_name = "writeDataByLocalIdentifier", .short_name = KWP_WDBLI},
        {.request = 0x3E, .reply = 0x7E, .inter_name = "testerPresent", .short_name = KWP_TP},
        {0, 0}};

const kwp_reply kwp_reply_table[] =
    {
        {.reply = 0x10, .inter_name = "generalReject", .short_name = GR},
        {.reply = 0x11, .inter_name = "serviceNotSupported", .short_name = SNS},
        {.reply = 0x12, .inter_name = "subFunctionNotSupported-invalidFormat", .short_name = SFNS_IF},
        {.reply = 0x21, .inter_name = "busy-RepeatRequest", .short_name = B_RR},
        {.reply = 0x31, .inter_name = "requestOutOfRange", .short_name = ROOR},
        {.reply = 0x72, .inter_name = "transferAborted", .short_name = GR},
        {.reply = 0x77, .inter_name = "blockTransferDataChecksumError", .short_name = BTDCE},
        {0, 0}};

const kwp_identificator *kwp_message::get_ident()
{
  uint8_t id = data[header.type - 1];
  printf("Searching for 0x%X\r\n", id);  
  for (auto i = 0; kwp_table[i].request; ++i)
  {
    if (kwp_table[i].request == id || kwp_table[i].reply == id)
    {
      printf("found %s\r\n", kwp_table[i].inter_name);
      return &kwp_table[i];
    }
  }
  return nullptr;
}

void kwp_message::add_ident(const kwp_identificator &id)
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
  length += size;
}

void kwp_message::print()
{
  printf("Message header: 0x%X 0x%X 0x%X\r\n", header.fmt, header.tgt, header.src);
  printf("Message of type 0x%X, length %u [CRC: 0x%X]\r\n",
         header.type,
         header.fmt & 0b00111111,
         checksum);
}

void kwp_message::update_format()
{
  header.fmt |= 1 << 7;
  header.fmt |= length & 0b00111111;
}

bool kwp_identificator::bad() const
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
  update_format();
  data[0] = header.fmt;
  data[1] = header.tgt;
  data[2] = header.src;
  if (header.type == HEADER_TYPE_4)
  {
    data[3] = length;
  }
}

uint8_t kwp_header::get_sum()
{
  return fmt + tgt + src + len;
}

} // namespace KWP2000