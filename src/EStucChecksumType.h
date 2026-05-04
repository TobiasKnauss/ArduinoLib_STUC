#ifndef EStucChecksumType_h
#define EStucChecksumType_h

#include <Arduino.h>

enum class EStucChecksumType : uint8_t
{
  None  = 0,
  CRC8  = 1,
  CRC16 = 2,
  CRC32 = 3
};

#endif
