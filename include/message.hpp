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
    uint8_t compl_1;
    uint8_t compl_2;
    uint8_t status_1;
    uint8_t status_2;
    uint8_t dtc_1;
    uint8_t dtc_2;
    uint8_t dtc_3;
    uint8_t dtc_4;
    uint8_t cwt;
    uint8_t afr;
    uint8_t tps;
    uint8_t rpm;
    uint8_t idle_rpm;
    uint8_t iac;
    uint8_t correction;
    uint8_t adv;
    uint8_t speed;
    uint8_t voltage;
    uint8_t lamda_state;
    uint16_t inj;
    uint16_t maf;
    uint16_t caf;
    uint16_t hff;
    uint16_t tff;
    uint16_t eeprom_crc;
  };
  uint8_t tab[36];
} RLI_ASS_tab;

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
