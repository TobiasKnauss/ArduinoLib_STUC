#include "StucData.h"

//--------------------------------------------------------------------
StucData::StucData ()
{
}

//--------------------------------------------------------------------
StucData::StucData (EStucAction i_Action,
                    uint32_t    i_RemoteDeviceId,
                    uint16_t    i_CommandId)
{
  Action         = i_Action;
  CommandId      = i_CommandId;
  RemoteDeviceId = i_RemoteDeviceId;
}

//--------------------------------------------------------------------
StucData::StucData (EStucAction         i_Action,
                    uint32_t            i_RemoteDeviceId,
                    uint32_t            i_MessageId,
                    uint32_t            i_Timestamp,
                    uint16_t            i_CommandId,
                    EStucMessageResult  i_MessageResult)
{
  Action         = i_Action;
  CommandId      = i_CommandId;
  MessageId      = i_MessageId;
  MessageResult  = i_MessageResult;
  RemoteDeviceId = i_RemoteDeviceId;
  Timestamp      = i_Timestamp;
}

//--------------------------------------------------------------------
void StucData::Clear ()
{
  Action         = EStucAction::None;
  CommandId      = 0;
  MessageId      = 0;
  MessageResult  = EStucMessageResult::None;
  RemoteDeviceId = 0;
  Timestamp      = 0;
}

//--------------------------------------------------------------------
void StucData::SetPayloadInfo (uint8_t* i_pPayloadBuffer,
                               uint8_t  i_PayloadBufferLength,
                               uint8_t  i_PayloadLength)
{
  pPayloadBuffer      = i_pPayloadBuffer;
  PayloadBufferLength = i_PayloadBufferLength;
  PayloadLength       = i_PayloadLength;
}
