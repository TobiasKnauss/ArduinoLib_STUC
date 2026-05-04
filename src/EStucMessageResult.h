#ifndef EStucMessageResult_h
#define EStucMessageResult_h

#include <Arduino.h>

enum class EStucMessageResult : uint8_t
{
  None       = 0,
  Success    = 1,
  InProgress = 2,
};

#endif
