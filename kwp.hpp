#pragma once

namespace KWP2000
{

const uint8_t ECU_ADDRESS = 0x10;         //SAE J2178
const uint8_t TESTER_ADDRESS = 0xF1;      //SAE J2178
const uint8_t IMMOBILIZER_ADDRESS = 0xC0; //SAE J2178
const uint8_t KWP_HEADER_PH_ADDR = 2;
const uint8_t KWP_MAX_DATA_SIZE = 128;

typedef enum kwp_short_name
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
  KWP_TP
} kwp_short_name;

typedef enum kwp_reply_short_name
{
  GR = 1,
  SNS,
  SFNS_IF,
  B_RR,
  ROOR,
  TA,
  BTDCE
} kwp_reply_short_name;

typedef struct
{
  uint8_t request;
  uint8_t reply;
  const char *inter_name;
  const kwp_short_name short_name;
} kwp_ident;

extern const kwp_ident kwp_table[];
typedef struct
{  
  uint8_t reply;
  const char *inter_name;
  const kwp_reply_short_name short_name;
} kwp_reply;

extern const kwp_reply kwp_reply_table[];

typedef enum kwp_header_type
{
  HEADER_TYPE_3 = 3,
  HEADER_TYPE_4 = 4
} kwp_header_type;

typedef struct kwp_header
{
  uint8_t fmt = 0;
  uint8_t tgt = ECU_ADDRESS;
  uint8_t src = TESTER_ADDRESS;
  uint8_t len = 0;
  kwp_header_type type = HEADER_TYPE_3;
  uint8_t get_sum();
} kwp_header;

typedef struct kwp_message
{
  kwp_header header;
  uint8_t data[KWP_MAX_DATA_SIZE + 4 + 1] = {0}; //Data + header + CRC
  uint8_t checksum = 0;
  uint8_t length = 0;
  void add_ident(const kwp_ident &id);
  void update_header();
  void update_crc();  
  void update_format();
} kwp_message;

class kwpTester
{
public:
  kwpTester(uint8_t commport, uint32_t baudrate);
  ~kwpTester();

public:
  bool start();

private:
  HANDLE open_port(uint8_t commport, uint32_t baudrate);
  void close_port(HANDLE commport);
  void send_message(kwp_message &message);
  kwp_message read_response();

private:
  const kwp_ident *find_ident(const kwp_short_name &name);
  const kwp_ident *find_ident(const uint8_t &code);
  kwp_message msg;
  HANDLE port;
};

} //namespace KWP2000