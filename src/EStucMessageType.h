#ifndef EStucMessageType_h
#define EStucMessageType_h

#include <Arduino.h>

enum class EStucMessageType : uint8_t
{
  None    = 0,
  Request = 1,
  Reply   = 2
};

#endif
