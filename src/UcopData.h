#ifndef UcopData_h
#define UcopData_h

#include <Arduino.h>

#include "UCOP.h"

struct UcopData
{
//==================== Fields ====================
  bool                  ActionIsWrite       = false;
  uint16_t              CommandId           = 0;
  uint32_t              MessageId           = 0;
  UCOP::EMessageResult  MessageResult       = UCOP::EMessageResult::None;
  uint32_t              RemoteDeviceId      = 0;
  uint32_t              Timestamp           = 0;
  uint8_t*              pPayloadBuffer      = 0;
  uint8_t               PayloadBufferLength = 0;
  uint8_t               PayloadLength       = 0;

//==================== Constructors ====================
  UcopData ();

  UcopData (bool      i_ActionIsWrite,
            uint32_t  i_RemoteDeviceId,
            uint16_t  i_CommandId);

  UcopData (bool                  i_ActionIsWrite,
            uint32_t              i_RemoteDeviceId,
            uint32_t              i_MessageId,
            uint32_t              i_Timestamp,
            uint16_t              i_CommandId,
            UCOP::EMessageResult  i_MessageResult);

//==================== Public Methods ====================
  void Clear ();
  
  void SetPayloadInfo (uint8_t* i_pPayloadBuffer,
                       uint8_t  i_PayloadBufferLength,
                       uint8_t  i_PayloadLength = 0);
};

#endif
