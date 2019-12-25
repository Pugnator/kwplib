#pragma once


typedef struct obd_fault_code
{
  const char type;
  const char family;
  const uint16_t code;
  const char[] description;  
}obd_fault_code;


extern obd_fault_code obd_table[];