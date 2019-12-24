#pragma once
#include <stdint.h>
#include <string>
#include <cstring>
#include <memory>

namespace KWP2000
{

const uint8_t ECU_ADDRESS = 0x10;         //SAE J2178
const uint8_t TESTER_ADDRESS = 0xF1;      //SAE J2178
const uint8_t IMMOBILIZER_ADDRESS = 0xC0; //SAE J2178
const uint8_t KWP_HEADER_PH_ADDR = 2;
const uint8_t KWP_MAX_DATA_SIZE = 128;

typedef enum service_alias
{
  KWP_STC = 1,
  KWP_SPC,
  KWP_STDS,
  KWP_SPDS,
  KWP_ER,
  KWP_CDI,
  KWP_RDTCBS,
  KWP_REI,
  KWP_RDBLI,
  KWP_RMBA,
  KWP_IOCBLI,
  KWP_WDBLI,
  KWP_TP,
  KWP_NR
} service_alias;

typedef enum reply_alias
{
  GR = 1,
  SNS,
  SFNS_IF,
  B_RR,
  ROOR,
  TA,
  BTDCE
} reply_alias;

typedef struct
{
  uint8_t request;
  uint8_t reply;
  const char *sae_name;
  const service_alias short_name;
  bool bad() const;
} kwp_service;

extern const kwp_service service_ids[];
typedef struct
{
  uint8_t reply;
  const char *sae_name;
  const reply_alias short_name;
} kwp_reply;

extern const kwp_reply kwp_reply_table[];

typedef enum kwp_header_type
{
  HEADER_SHORT = 3,
  HEADER_LONG = 4
} kwp_header_type;

typedef struct kwp_header
{
  uint8_t format = 0;
  uint8_t target = ECU_ADDRESS;
  uint8_t source = TESTER_ADDRESS;
  uint8_t length = 0;
  kwp_header_type type = HEADER_SHORT;
  uint8_t get_sum();
} kwp_header;

typedef struct kwp_message
{
  kwp_header header;
  uint8_t data[KWP_MAX_DATA_SIZE + 4 + 1] = {0}; //Data + header + CRC
  uint8_t checksum = 0;
  uint8_t length = 0;
  uint8_t source;
  void add_ident(const kwp_service &id);
  void add_param(const uint8_t &param);
  const kwp_service *get_ident();
  void add_payload(uint8_t *_data, uint8_t size);
  void update_header();
  void update_crc();
  void update_format_byte();
  void print();
} kwp_message;

} // namespace KWP2000
