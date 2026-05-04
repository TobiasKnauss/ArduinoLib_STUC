#ifndef StucData_h
#define StucData_h

#include <Arduino.h>
#include "EStucAction.h"
#include "EStucMessageResult.h"

struct StucData
{
//==================== Fields ====================
  EStucAction         Action              = EStucAction::None;
  uint16_t            CommandId           = 0;
  uint32_t            MessageId           = 0;
  EStucMessageResult  MessageResult       = EStucMessageResult::None;
  uint32_t            RemoteDeviceId      = 0;
  uint32_t            Timestamp           = 0;
  uint8_t*            pPayloadBuffer      = 0;
  uint8_t             PayloadBufferLength = 0;
  uint8_t             PayloadLength       = 0;

//==================== Constructors ====================
  StucData ();

  StucData (EStucAction i_Action,
            uint32_t    i_RemoteDeviceId,
            uint16_t    i_CommandId);

  StucData (EStucAction         i_Action,
            uint32_t            i_RemoteDeviceId,
            uint32_t            i_MessageId,
            uint32_t            i_Timestamp,
            uint16_t            i_CommandId,
            EStucMessageResult  i_MessageResult);

//==================== Public Methods ====================
  void Clear ();
  
  void SetPayloadInfo (uint8_t* i_pPayloadBuffer,
                       uint8_t  i_PayloadBufferLength,
                       uint8_t  i_PayloadLength = 0);
};

#endif
