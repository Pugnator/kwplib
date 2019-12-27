#pragma once
#include <cstdint>

typedef struct dtc_description
{
  const uint16_t code;
  const char *description;
} dtc_description;

extern dtc_description dtc_table[];

typedef struct dtc
{
  char *group;
  uint16_t code;
} dtc;
