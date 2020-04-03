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
const uint8_t NORMAL_BAUDRATE = 0x0A;
const uint8_t HIGH_BAUDRATE = 0x26;
const uint8_t ENHANCED_BAUDRATE = 0x39;

typedef union ECU_identification {
  struct __attribute__((packed, aligned(128)))
  {
    uint8_t VIN[19];
    uint8_t vehicleManufacturerECUHardwareNumber[16];
    uint8_t systemSupplierECUHardwareNumber[10];
    uint8_t systemSupplierECUSoftwareNumber[10];
    uint8_t systemNameOrEngineType[15];
    uint8_t repairShopCode[7];
    uint8_t ProgrammingDate[10]; //(ДД-ММ-ГГГГ
    uint8_t vehicleManufacturerECUIdentifier[8];
  };
  uint8_t data[128] = {'.'};
} ECU_identification_table;

typedef union RLI_ASS_tab {
  struct
  {
    uint8_t equip_1;     //#1
    uint8_t equip_2;     //#2
    uint8_t status_1;    //#3
    uint8_t status_2;    //#4
    uint8_t dtc_1;       //#5
    uint8_t dtc_2;       //#6
    uint8_t dtc_3;       //#7
    uint8_t dtc_4;       //#8
    uint8_t cwt;         //#9
    uint8_t afr;         //#10
    uint8_t tps;         //#11
    uint8_t rpm;         //#12
    uint8_t idle_rpm;    //#13
    uint8_t needed_iac;  //#14
    uint8_t iac;         //#15
    uint8_t correction;  //#16
    int8_t adv;          //#17
    uint8_t speed;       //#18
    uint8_t voltage;     //#19
    uint8_t lamda_state; //#20
    uint16_t inj;        //#21-22
    uint16_t maf;        //#24-24
    uint16_t caf;        //#25-26
    uint16_t hff;        //#27-28
    uint16_t tff;        //#29-30
    uint16_t eeprom_crc; //31-32
  };
  uint8_t tab[34];

  int16_t get_water_temp();
  uint16_t get_throttle();
  uint16_t get_speed();
  double get_afr();
  uint16_t get_rpm();
  uint16_t get_idle_rpm();
  double get_correction();
  double get_advance();
  double get_battery();
  double get_nbo_voltage();
} RLI_ASS_tab;

typedef union equipment_word_1 {
  struct
  {
    uint8_t flag : 1;
  };
  uint8_t byte;
} equipment_word_1;

typedef union equipment_word_2 {
  struct
  {
  };
  uint8_t byte;
} equipment_word_2;

typedef union run_mode_1 {
  struct
  {
  };
  uint8_t byte;
} run_mode_1;

typedef union run_mode_2 {
  struct
  {
  };
  uint8_t byte;
} run_mode_2;

typedef union dtc_flags_1 {
  struct
  {
  };
  uint8_t byte;
} dtc_flags_1;

typedef union dtc_flags_2 {
  struct
  {
  };
  uint8_t byte;
} dtc_flags_2;

typedef union dtc_flags_3 {
  struct
  {
  };
  uint8_t byte;
} dtc_flags_3;

typedef union dtc_flags_4 {
  struct
  {
  };
  uint8_t byte;
} dtc_flags_4;

extern ECU_identification_table ECU_ID;

typedef enum identificationOption
{
  ECUIDT = 0x80,
  VIN = 0x90,
  VMECUHN = 0x91,
  SSECUHN = 0x92,
  SSECUSN = 0x94,
  SNOET = 0x97,
  RSC = 0x98,
  PD = 0x99,
  VMECUID = 0x9A
} identificationOption;

typedef enum recordLocalIdentifier
{
  RLI_ASS = 0x1,
  RLI_EOL = 0x2,
  RLI_FT = 0x3,
  RLI_IR = 0xA0,
  RLI_BSN = 0xA1,
  RLI_ESN = 0xA2,
  RLI_MD = 0xA3
} recordLocalIdentifier;

typedef enum service_mnemonic
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
} service_mnemonic;

typedef enum reply_nmemonic
{
  GR = 1,
  SNS,
  SFNS_IF,
  B_RR,
  ROOR,
  TA,
  BTDCE
} reply_nmemonic;

typedef struct
{
  uint8_t request;
  uint8_t reply;
  const char *sae_name;
  const service_mnemonic short_name;
  bool bad() const;
} kwp_service;

extern const kwp_service service_ids[];
typedef struct
{
  uint8_t reply;
  const char *sae_name;
  const reply_nmemonic short_name;
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

  void add_service_id(const kwp_service &id);
  void add_param(const uint8_t &param);
  const kwp_service *get_service();
  void add_payload(uint8_t *_data, uint8_t size);
  void update_header();
  void update_crc();
  void update_format_byte();
  void print();
} kwp_message;

} // namespace KWP2000
