#include <message.hpp>

namespace KWP2000
{

ECU_identification_table ECU_ID;

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