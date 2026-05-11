#include "UcopData.h"

//--------------------------------------------------------------------
UcopData::UcopData ()
{
}

//--------------------------------------------------------------------
UcopData::UcopData (bool      i_ActionIsWrite,
                    uint32_t  i_RemoteDeviceId,
                    uint16_t  i_CommandId)
{
  ActionIsWrite  = i_ActionIsWrite;
  CommandId      = i_CommandId;
  RemoteDeviceId = i_RemoteDeviceId;
}

//--------------------------------------------------------------------
UcopData::UcopData (bool                  i_ActionIsWrite,
                    uint32_t              i_RemoteDeviceId,
                    uint32_t              i_MessageId,
                    uint32_t              i_Timestamp,
                    uint16_t              i_CommandId,
                    UCOP::EMessageResult  i_MessageResult)
{
  ActionIsWrite  = i_ActionIsWrite;
  CommandId      = i_CommandId;
  MessageId      = i_MessageId;
  MessageResult  = i_MessageResult;
  RemoteDeviceId = i_RemoteDeviceId;
  Timestamp      = i_Timestamp;
}

//--------------------------------------------------------------------
void UcopData::Clear ()
{
  ActionIsWrite  = false;
  CommandId      = 0;
  MessageId      = 0;
  MessageResult  = UCOP::EMessageResult::None;
  RemoteDeviceId = 0;
  Timestamp      = 0;
}

//--------------------------------------------------------------------
void UcopData::SetPayloadInfo (uint8_t* i_pPayloadBuffer,
                               uint8_t  i_PayloadBufferLength,
                               uint8_t  i_PayloadLength)
{
  pPayloadBuffer      = i_pPayloadBuffer;
  PayloadBufferLength = i_PayloadBufferLength;
  PayloadLength       = i_PayloadLength;
}
