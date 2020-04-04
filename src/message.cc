#include <client.hpp>
#include <message.hpp>

namespace KWP2000
{

ECU_identification_table ECU_ID;

const kwp_service *kwp_message::get_service()
{
  uint8_t id = data[header.type - 1];
  #if defined (WIN32) && defined(__DEBUG)
  printf("Searching for 0x%X\r\n", id);
  #endif
  for (auto i = 0; service_ids[i].request; ++i)
  {
    if (service_ids[i].request == id || service_ids[i].reply == id)
    {      
      DEBUG_LOG("found %s\r\n", service_ids[i].sae_name);      
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
  DEBUG_LOG("Param: 0x%X\r\n", param); 
  data[header.type + length++] = param;
}

void kwp_message::add_payload(uint8_t *payload, uint8_t size)
{
  memcpy(data + header.type - 1, payload, size);
}

void kwp_message::print()
{  
  DEBUG_LOG("Message header: 0x%X 0x%X 0x%X\r\n", header.format, header.target, header.source);
  DEBUG_LOG("Message of type 0x%X, length 0x%X [CRC: 0x%X]\r\n",
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

int16_t RLI_ASS_tab::get_water_temp()
{
  return cwt - 40; //N=E-40 [C]
}

uint16_t RLI_ASS_tab::get_throttle()
{
  return tps;
}
uint16_t RLI_ASS_tab::get_speed()
{
  return speed;
}
double RLI_ASS_tab::get_afr()
{
  return 14.7 * (afr + 128) / 256; //N=14.7*(E+128)/256
}
uint16_t RLI_ASS_tab::get_rpm() //N=E*40 [об/мин]
{
  return rpm * 40;
}
uint16_t RLI_ASS_tab::get_idle_rpm() //N=E*10 [об/мин]
{
  return rpm * 10;
}
double RLI_ASS_tab::get_correction() //N=(E+128)/256
{
  return (correction + 128) / 256;
}
double RLI_ASS_tab::get_advance() //N=E/2 [гр.КВ], где E-знаковое
{
  return adv / 2;
}
double RLI_ASS_tab::get_battery() //N=5.2 +E*0.05 [В]
{
  return 5.2 + (voltage * 0.05);
}
double RLI_ASS_tab::get_nbo_voltage() //N=1.25*(Е/256) [В]
{
  return 1.25 * (lamda_state / 256);
}

} // namespace KWP2000