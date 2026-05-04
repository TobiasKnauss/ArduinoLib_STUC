#ifndef EStucAction_h
#define EStucAction_h

#include <Arduino.h>

enum class EStucAction : uint8_t
{
  None  = 0,
  Read  = 1,
  Write = 2
};

#endif
