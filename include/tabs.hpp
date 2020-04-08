
#pragma once

namespace KWP2000
{
typedef union ECU_identification {
  struct __attribute__((packed, aligned(128)))
  {
    uint8_t VIN[19];
    uint8_t vehicleManufacturerECUHardwareNumber[16];
    uint8_t systemSupplierECUHardwareNumber[10];
    uint8_t systemSupplierECUSoftwareNumber[10];
    uint8_t systemNameOrEngineType[15];
    uint8_t repairShopCode[7];
    uint8_t ProgrammingDate[10]; //DD-MM-YYYY
    uint8_t vehicleManufacturerECUIdentifier[8];
  };
  uint8_t data[128] = {'.'};
} ECU_identification_table;
extern ECU_identification_table ECU_ID;

typedef union RLI_ASS_tab {
  struct
  {
    uint8_t equip_1;     //#0
    uint8_t equip_2;     //#1
    uint8_t status_1;    //#2
    uint8_t status_2;    //#3
    uint8_t dtc_1;       //#4
    uint8_t dtc_2;       //#5
    uint8_t dtc_3;       //#6
    uint8_t dtc_4;       //#7
    uint8_t cwt;         //#8
    uint8_t afr;         //#9
    uint8_t tps;         //#10
    uint8_t rpm;         //#11
    uint8_t idle_rpm;    //#12
    uint8_t needed_iac;  //#13
    uint8_t iac;         //#14
    uint8_t correction;  //#15
    int8_t adv;          //#16
    uint8_t speed;       //#17
    uint8_t voltage;     //#18
    uint8_t lamda_state; //#19
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

typedef union RLI_OLT_tab {
  struct
  {
    uint8_t equip_1;     //#0
    uint8_t equip_2;     //#1
    uint8_t status_1;    //#2
    uint8_t status_2;    //#3
    uint8_t dtc_1;       //#4
    uint8_t adv;       //#5
    uint8_t dtc_3;       //#6
    uint8_t dtc_4;       //#7
    uint8_t cwt;         //#8+
    uint8_t afr;         //#9
    uint8_t a1;         //#10
    uint8_t tps;         //#11+
    uint8_t idle_rpm;    //#12
    uint8_t needed_iac;  //#13
    uint8_t rpm1;         //#14+
    uint8_t rpm2;        //#15+
    uint8_t temp7;          //#16
    uint8_t voltage;       //#17
    uint8_t temp0;     //#18
    uint8_t temp1;       //#19
    uint8_t temp2;       //#20
    uint8_t temp3;       //#21
    uint8_t temp4;       //#22
    uint8_t temp5;       //#23
    uint8_t temp6;       //#24
    uint8_t speed;       //#25    
    uint8_t iat;         //#26    
  };
  uint8_t tab[128];

  int16_t get_water_temp();
  int16_t get_air_temp();
  uint16_t get_throttle();
  uint16_t get_speed();
  double get_afr();
  uint16_t get_rpm();
  uint16_t get_idle_rpm();
  double get_correction();
  double get_advance();
  double get_battery();  
} RLI_OLT_tab;

typedef union RLI_TRS_tab {
  struct
  {
    uint8_t equip_1;     //#0
    uint8_t equip_2;     //#1
    uint8_t status_1;    //#2
    uint8_t status_2;    //#3
    uint8_t dtc_1;       //#4
    uint8_t adv;       //#5
    uint8_t dtc_3;       //#6
    uint8_t dtc_4;       //#7
    uint8_t cwt;         //#8+
    uint8_t afr;         //#9
    uint8_t a1;         //#10
    uint8_t tps;         //#11+
    uint8_t idle_rpm;    //#12
    uint8_t needed_iac;  //#13
    uint8_t rpm1;         //#14+
    uint8_t rpm2;        //#15+
    uint8_t temp7;          //#16
    uint8_t voltage;       //#17
    uint8_t temp0;     //#18
    uint8_t temp1;       //#19
    uint8_t temp2;       //#20
    uint8_t temp3;       //#21
    uint8_t temp4;       //#22
    uint8_t temp5;       //#23
    uint8_t temp6;       //#24
    uint8_t speed;       //#25    
    uint8_t iat;         //#26    
  };
  uint8_t tab[128];

  int16_t get_water_temp();
  int16_t get_air_temp();
  uint16_t get_throttle();
  uint16_t get_speed();
  double get_afr();
  uint16_t get_rpm();
  uint16_t get_idle_rpm();
  double get_correction();
  double get_advance();
  double get_battery();  
} RLI_TRS_tab;
}