#include <EEPROM.h>
#include "UCOP.h"
#include "UCOPData.h"

//--------------------------------------------------------------------
#define X(name) const char UCOP::_EResult_##name[] PROGMEM = #name;
#include "UCOP_EResult_failures.h"
#undef X

//--------------------------------------------------------------------
#define X(name) _EResult_##name,
const char* const UCOP::c_EResult_ClassFailures_Names[] PROGMEM =
{
  #include "UCOP_EResult_failures.h"
};
#undef X

//--------------------------------------------------------------------
#define X(name) const char UCOP::_EMessageResult_##name[] PROGMEM = #name;
#include "UCOP_EMessageResult_results.h"
#include "UCOP_EMessageResult_failures.h"
#undef X

//--------------------------------------------------------------------
#define X(name) _EMessageResult_##name,
const char* const UCOP::c_EMessageResult_Results_Names[] PROGMEM =
{
#include "UCOP_EMessageResult_results.h"
};
const char* const UCOP::c_EMessageResult_Failures_Names[] PROGMEM =
{
#include "UCOP_EMessageResult_failures.h"
};
#undef X

//--------------------------------------------------------------------
UCOP::UCOP (bool          i_DeviceIdsUsed,
            bool          i_MessageIdUsed,
            bool          i_TimestampUsed,
            uint32_t      i_DeviceId,
            EChecksumType i_ChecksumType,
            ::EResult&    o_Result)
{
  o_Result = CommonConstructor (i_DeviceIdsUsed,
                                i_MessageIdUsed,
                                i_TimestampUsed,
                                i_DeviceId,
                                i_ChecksumType);
}

//--------------------------------------------------------------------
UCOP::UCOP (uint16_t    i_EepromAddress,
            ::EResult&  o_Result)
{
  o_Result = ReadConfigFromEEPROM (i_EepromAddress);
}

//--------------------------------------------------------------------
::EResult UCOP::CommonConstructor ( bool          i_DeviceIdsUsed,
                                    bool          i_MessageIdUsed,
                                    bool          i_TimestampUsed,
                                    uint32_t      i_DeviceId,
                                    EChecksumType i_ChecksumType)
{
  if (i_DeviceId == 0)
    return ::EResult::FAIL_Device_IdInvalid;
  if (!IsChecksumValid (i_ChecksumType))
    return ::EResult::FAIL_Device_ConfigInvalid;

  m_DeviceIdsUsed = i_DeviceIdsUsed;
  m_MessageIdUsed = i_MessageIdUsed;
  m_TimestampUsed = i_TimestampUsed;
  m_DeviceId      = i_DeviceId;
  m_ChecksumType  = i_ChecksumType;

  return ::EResult::SUCCESS;
}

//--------------------------------------------------------------------
uint8_t UCOP::GetChecksumLength (EChecksumType i_ChecksumType)
{
  switch (i_ChecksumType)
  {
  case EChecksumType::CRC8 : return 1;
  case EChecksumType::CRC16: return 2;
  case EChecksumType::CRC32: return 4;
  default:                   return 0;
  }
}

//--------------------------------------------------------------------
UCOP::EMessageResult UCOP::GetMessageResultForFunctionResult (::EResult i_Result)
{
  switch ((EResult)i_Result)
  {
  case EResult::FAIL_UCOP_Message_NotFound:                 return EMessageResult::None;
  case EResult::FAIL_UCOP_Message_ReceiverDeviceIdMismatch: return EMessageResult::None;
  case EResult::FAIL_UCOP_Message_SenderDeviceIdInvalid:    return EMessageResult::FAIL_DeviceIdInvalid;
  case EResult::FAIL_UCOP_Message_MessageIdInvalid:         return EMessageResult::FAIL_MessageIdInvalid;
  case EResult::FAIL_UCOP_Message_TimestampInvalid:         return EMessageResult::FAIL_TimestampInvalid;
  case EResult::FAIL_UCOP_Message_CommandIdInvalid:         return EMessageResult::FAIL_CommandIdInvalid;
  case EResult::FAIL_UCOP_Message_ResultWrong:              return EMessageResult::FAIL_ResultWrong;
  default:                                                  return EMessageResult::FAIL_InternalFailure;
  }
}

//--------------------------------------------------------------------
const __FlashStringHelper* UCOP::GetMessageResultText (EMessageResult i_MessageResult)
{
  if ((uint16_t)i_MessageResult < (uint16_t)EMessageResult::Dummy_FirstFailure)
    return (const __FlashStringHelper*)pgm_read_ptr(&c_EMessageResult_Results_Names[(uint16_t)i_MessageResult]);

  return (const __FlashStringHelper*)pgm_read_ptr(&c_EMessageResult_Failures_Names[(uint16_t)i_MessageResult - (uint16_t)EMessageResult::Dummy_FirstFailure - 1]);
}

//--------------------------------------------------------------------
const __FlashStringHelper* UCOP::GetResultText (::EResult i_Result)
{
  if ((uint16_t)i_Result < (uint16_t)EResult::Dummy_FirstClassFailure)
    return Result::GetText (i_Result);
  return (const __FlashStringHelper*)pgm_read_ptr(&c_EResult_ClassFailures_Names[(uint16_t)i_Result - (uint16_t)EResult::Dummy_FirstClassFailure - 1]);
}

//--------------------------------------------------------------------
bool UCOP::IsChecksumValid (EChecksumType i_ChecksumType)
{
  return i_ChecksumType == EChecksumType::None
      || GetChecksumLength (i_ChecksumType) > 0;
}

//--------------------------------------------------------------------
uint8_t UCOP::CalcHeaderSize ()
{
  return c_HeaderMinLength
       + (m_DeviceIdsUsed ? 8 : 0)
       + (m_MessageIdUsed ? 4 : 0)
       + (m_TimestampUsed ? 4 : 0);
}

//--------------------------------------------------------------------
uint8_t UCOP::CalcTrailerSize ()
{
  return c_TrailerMinLength
       + GetChecksumLength (m_ChecksumType);
}

//--------------------------------------------------------------------
EResult UCOP::ComposeReply (UCOPData& i_Data,
                            byte*     i_pMessageBuffer,
                            byte      i_MessageBufferLength,
                            uint16_t& o_MessageLength)
{
  return ComposeMessage (i_Data,
                         i_pMessageBuffer,
                         i_MessageBufferLength,
                         o_MessageLength,
                         true);
}

//--------------------------------------------------------------------
EResult UCOP::ComposeRequest (UCOPData& i_Data,
                              uint8_t*  i_pMessageBuffer,
                              uint8_t   i_MessageBufferLength,
                              uint16_t& o_MessageLength)
{
  return ComposeMessage (i_Data,
                         i_pMessageBuffer,
                         i_MessageBufferLength,
                         o_MessageLength,
                         false);
}

//--------------------------------------------------------------------
void UCOP::PrintConfig ()
{
  Serial << F("DeviceIdsUsed = ") << m_DeviceIdsUsed << endl;
  Serial << F("MessageIdUsed = ") << m_MessageIdUsed << endl;
  Serial << F("TimestampUsed = ") << m_TimestampUsed << endl;
  Serial << F("DeviceId      = ") << m_DeviceId << " = 0x" << _HEX8 (m_DeviceId) << endl;
  Serial << F("ChecksumType  = ") << (uint8_t)m_ChecksumType << endl;
}

//--------------------------------------------------------------------
EResult UCOP::SearchMessage (uint8_t*  i_pRingBuffer,
                             uint16_t  i_RingBufferLength,
                             uint16_t& io_RingBufferStartIndex,
                             UCOPData& io_Data,
                             bool&     o_MessageTypeIsReply,
                             uint8_t&  o_MessageLength)
{
  if (i_pRingBuffer == nullptr)
    return ::EResult::FAIL_Pointer_IsZero;
  if (i_RingBufferLength < c_MessageMinLength)
    return ::EResult::FAIL_Buffer_TooSmall;
  if (io_RingBufferStartIndex >= i_RingBufferLength)
    return ::EResult::FAIL_Buffer_IndexOutsideRange;

  io_Data.Clear ();
  io_Data.PayloadLength = 0;
  o_MessageTypeIsReply = false;
  o_MessageLength      = 0;

  // Execute multiple attempts to search the start of the message:
  // - In each attempt, the message start ID is searched by looping over the buffer.
  // - When the message start ID is found, it is assumed that a complete message follows.
  // - The found possible message is analyzed. If it is a valid message, it is evaluated. If not, the next search attempt is started one byte after the previously found message start ID.

  for (uint16_t searchIndex = 0; searchIndex < i_RingBufferLength; searchIndex++)
  {
    uint8_t* pSearch = i_pRingBuffer + io_RingBufferStartIndex + searchIndex;
    if (pSearch >= i_pRingBuffer + i_RingBufferLength)
      pSearch -= i_RingBufferLength;
    
    ::EResult result = ::EResult::InProgress;

    //========== Header ==========
    // STX
    if (*pSearch != c_MessageStartID)
      continue;

    uint8_t  valueUI8;
    uint16_t valueUI16;
    uint8_t* pAnalyze = pSearch;
    RingBuffer_IncrementPointer (i_pRingBuffer, i_RingBufferLength, pAnalyze);

    // Version
    uint8_t version;
    if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, version))
      return ::EResult::FAIL_Buffer_GetValue;
    if (version != c_Version)
      continue;
    
    // Flags
    uint8_t flags = 0;
    if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, flags))
      return ::EResult::FAIL_Buffer_GetValue;

    // Flag: MessageType
    o_MessageTypeIsReply = (flags >> c_FlagIndex_MessageType) & 0x01;

    // Flag: Action
    io_Data.ActionIsWrite = (flags >> c_FlagIndex_Action) & 0x01;

    // Flag: DeviceIdsUsed
    if (flags & 1 << c_FlagIndex_DeviceIdsUsed)
    {
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, io_Data.RemoteDeviceId))
        return ::EResult::FAIL_Buffer_GetValue;
      if (io_Data.RemoteDeviceId == 0
      &&  result == ::EResult::InProgress)
        result = (::EResult)EResult::FAIL_UCOP_Message_SenderDeviceIdInvalid;
      
      uint32_t ownDeviceId;
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, ownDeviceId))
        return ::EResult::FAIL_Buffer_GetValue;
      if (ownDeviceId != m_DeviceId
      &&  result == ::EResult::InProgress)
        result = (::EResult)EResult::FAIL_UCOP_Message_ReceiverDeviceIdMismatch;
    }

    // Flag: MessageIdUsed
    if (flags & 1 << c_FlagIndex_MessageIdUsed)
    {
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, io_Data.MessageId))
        return ::EResult::FAIL_Buffer_GetValue;
      if (io_Data.MessageId == 0
      &&  result == ::EResult::InProgress)
        result = (::EResult)EResult::FAIL_UCOP_Message_MessageIdInvalid;
    }

    // Flag: TimestampUsed
    if (flags & 1 << c_FlagIndex_TimestampUsed)
    {
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, io_Data.Timestamp))
        return ::EResult::FAIL_Buffer_GetValue;
      if (io_Data.Timestamp == 0
      &&  result == ::EResult::InProgress)
        result = (::EResult)EResult::FAIL_UCOP_Message_TimestampInvalid;
    }

    // Command ID
    if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, io_Data.CommandId))
      return ::EResult::FAIL_Buffer_GetValue;
    if (io_Data.CommandId == 0
    &&  result == ::EResult::InProgress)
      result = (::EResult)EResult::FAIL_UCOP_Message_CommandIdInvalid;

    // Result
    if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, valueUI8))
      return ::EResult::FAIL_Buffer_GetValue;
    io_Data.MessageResult = (EMessageResult)valueUI8;
    if ((io_Data.MessageResult == EMessageResult::None) == o_MessageTypeIsReply  // failure if (reply and no result) or (request and result)
    &&  result == ::EResult::InProgress)
      result = (::EResult)EResult::FAIL_UCOP_Message_ResultWrong;

    // Payload data length
    if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, io_Data.PayloadLength))
      return ::EResult::FAIL_Buffer_GetValue;
    if (io_Data.PayloadLength > 0)
    {
      if (io_Data.pPayloadBuffer == nullptr)
        return ::EResult::FAIL_Pointer_IsZero;
      if (io_Data.PayloadBufferLength < io_Data.PayloadLength)
        return ::EResult::FAIL_Buffer_TooSmall;
    }

    //========== Payload ==========
    // Payload data length
    if (!RingBuffer_GetBytesAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, io_Data.PayloadLength, io_Data.pPayloadBuffer))
      return ::EResult::FAIL_Buffer_GetBytes;

    //========== Trailer ==========
    // Checksum:  header without STX, payload data
    EChecksumType checksumType = (EChecksumType)((flags >> c_FlagIndex_ChecksumType) & 0x03);
    uint32_t checksumCalculated = 0;
    uint32_t checksumFromMessage = 0;
    switch (checksumType)
    {
    case EChecksumType::None:
      break;
    case EChecksumType::CRC8:
      if (pAnalyze > pSearch)
        checksumCalculated = m_Crc8.maxim (pSearch + 1, pAnalyze - pSearch - 1);
      else
      {
                             m_Crc8.maxim     (pSearch + 1,   i_pRingBuffer + i_RingBufferLength - pSearch - 1);
        checksumCalculated = m_Crc8.maxim_upd (i_pRingBuffer, pAnalyze - i_pRingBuffer);
      }
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, valueUI8))
        return ::EResult::FAIL_Buffer_GetValue;
      checksumFromMessage = valueUI8;
      break;

    case EChecksumType::CRC16:
      if (pAnalyze > pSearch)
        checksumCalculated = m_Crc16.modbus (pSearch + 1, pAnalyze - pSearch - 1);
      else
      {
                             m_Crc16.modbus     (pSearch + 1,   i_pRingBuffer + i_RingBufferLength - pSearch - 1);
        checksumCalculated = m_Crc16.modbus_upd (i_pRingBuffer, pAnalyze - i_pRingBuffer);
      }
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, valueUI16))
        return ::EResult::FAIL_Buffer_GetValue;
      checksumFromMessage = valueUI16;
      break;

    case EChecksumType::CRC32:
      if (pAnalyze > pSearch)
        checksumCalculated = m_Crc32.crc32 (pSearch + 1, pAnalyze - pSearch - 1);
      else
      {
                             m_Crc32.crc32     (pSearch + 1,   i_pRingBuffer + i_RingBufferLength - pSearch - 1);
        checksumCalculated = m_Crc32.crc32_upd (i_pRingBuffer, pAnalyze - i_pRingBuffer);
      }
      if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, checksumFromMessage))
        return ::EResult::FAIL_Buffer_GetValue;
      break;
    }

    // ETX
    uint8_t messageEndID;
    if (!RingBuffer_GetValueAndMovePtr (i_pRingBuffer, i_RingBufferLength, pAnalyze, messageEndID))
      return ::EResult::FAIL_Buffer_GetValue;
    if (messageEndID == c_MessageEndID // message was found
    &&  checksumCalculated == checksumFromMessage) // message is valid
    {
      // clear the analyzed buffer part
      if (pAnalyze > pSearch)
      {
        o_MessageLength = pAnalyze - pSearch;
        memset (pSearch, 0xDD, o_MessageLength);
      }
      else  // message wraps around at buffer end
      {
        uint8_t len1 = i_pRingBuffer + i_RingBufferLength - pSearch;
        uint8_t len2 = pAnalyze - i_pRingBuffer;
        o_MessageLength = len1 + len2;
        memset (pSearch,       0xDD, len1);
        memset (i_pRingBuffer, 0xDD, len2);
      }

      io_RingBufferStartIndex = pAnalyze - i_pRingBuffer;
      return result == ::EResult::InProgress ? ::EResult::SUCCESS : result;
    }
    // else: Message End ID missing -> This was no message. Previously identified failures are invalid.
    //       or Checksum wrong -> The message content is incorrect. Previously identified failures are invalid.

    io_Data.Clear ();
    io_Data.PayloadLength = 0;
    o_MessageTypeIsReply  = false;
    o_MessageLength       = 0;
  }

  return (::EResult)EResult::FAIL_UCOP_Message_NotFound;
}

//--------------------------------------------------------------------
::EResult UCOP::WriteConfigToEEPROM (uint16_t i_Address)
{
  if (c_EepromConfigTotalSize + i_Address > EEPROM.length ())
    return ::EResult::FAIL_EEPROM_IndexOutsideRange;

  EEPROM.put (i_Address + 0, m_DeviceIdsUsed);
  EEPROM.put (i_Address + 1, m_MessageIdUsed);
  EEPROM.put (i_Address + 2, m_TimestampUsed);
  EEPROM.put (i_Address + 3, m_DeviceId);
  EEPROM.put (i_Address + 7, m_ChecksumType);

  uint8_t  byteValue = 0;
  uint16_t checksum = m_Crc16.modbus (&byteValue, 1);
  for (int index = 0; index < c_EepromConfigDataSize; index++)
  {
    byteValue = EEPROM.read (i_Address + index);
    checksum = m_Crc16.modbus_upd (&byteValue, 1);
  }
  EEPROM.put (i_Address + 8, checksum);

  return ::EResult::SUCCESS;
}

//--------------------------------------------------------------------
::EResult UCOP::ComposeMessage (UCOPData& i_Data,
                                uint8_t*  i_pMessageBuffer,
                                uint8_t   i_MessageBufferLength,
                                uint16_t& o_MessageLength,
                                bool      i_MessageIsReply)
{
  if (i_pMessageBuffer == nullptr)
    return ::EResult::FAIL_Pointer_IsZero;

  uint8_t headerSize  = CalcHeaderSize ();
  uint8_t trailerSize = CalcTrailerSize ();
  if (i_MessageBufferLength < headerSize + i_Data.PayloadLength + trailerSize)
    return ::EResult::FAIL_Buffer_TooSmall;

  bool copyPayload = false;
  if (i_Data.PayloadLength > 0)
  {
    if (i_Data.pPayloadBuffer == nullptr)
      return ::EResult::FAIL_Pointer_IsZero;
    if (i_Data.PayloadBufferLength < i_Data.PayloadLength)
      return ::EResult::FAIL_Buffer_TooSmall;

    copyPayload = i_Data.pPayloadBuffer != i_pMessageBuffer + headerSize; // Copy the payload data only if it is not yet at the correct position in the message buffer.
    if (copyPayload && CheckMemoryAreasOverlap (i_pMessageBuffer, i_MessageBufferLength, i_Data.pPayloadBuffer, i_Data.PayloadLength)) // If the payload must be copied and the memory areas overlap, then copying is not possible.
      return ::EResult::FAIL_Buffer_Overlap;
  }

  //========== Header ==========
  memset (i_pMessageBuffer, 0, headerSize);
  uint8_t* pMessageBuffer = i_pMessageBuffer;

  // STX
  *(pMessageBuffer++) = c_MessageStartID;

  // Version
  *(pMessageBuffer++) = c_Version;

  // Flags
  uint8_t flags = (byte)m_ChecksumType << 5;
  if (i_MessageIsReply    ) flags |= 1 << c_FlagIndex_MessageType;
  if (i_Data.ActionIsWrite) flags |= 1 << c_FlagIndex_Action;
  if (m_DeviceIdsUsed     ) flags |= 1 << c_FlagIndex_DeviceIdsUsed;
  if (m_MessageIdUsed     ) flags |= 1 << c_FlagIndex_MessageIdUsed;
  if (m_TimestampUsed     ) flags |= 1 << c_FlagIndex_TimestampUsed;
  *(pMessageBuffer++) = flags;

  // Sender and Receiver Device IDs
  if (m_DeviceIdsUsed)
  {
    memcpy (pMessageBuffer, &m_DeviceId, sizeof (m_DeviceId));
    pMessageBuffer += sizeof (m_DeviceId);
    memcpy (pMessageBuffer, &i_Data.RemoteDeviceId, sizeof (i_Data.RemoteDeviceId));
    pMessageBuffer += sizeof (i_Data.RemoteDeviceId);
  }

  // Message ID
  if (m_MessageIdUsed)
  {
    if (i_MessageIsReply)
    {
      memcpy (pMessageBuffer, &i_Data.MessageId, sizeof (i_Data.MessageId));
    }
    else
    {
      m_MessageId++;
      if (m_MessageId == 0)
        m_MessageId = 1;
      memcpy (pMessageBuffer, &m_MessageId, sizeof (m_MessageId));
    }
    pMessageBuffer += sizeof (m_MessageId);
  }

  // Timestamp
  if (m_TimestampUsed)
  {
    memcpy (pMessageBuffer, &i_Data.Timestamp, sizeof (i_Data.Timestamp));
    pMessageBuffer += sizeof (i_Data.Timestamp);
  }

  // Command ID
  memcpy (pMessageBuffer, &i_Data.CommandId, sizeof (i_Data.CommandId));
  pMessageBuffer += sizeof (i_Data.CommandId);

  // Message Result
  *pMessageBuffer++ = i_MessageIsReply ? (uint8_t)i_Data.MessageResult : 0;

  // Payload data length
  *pMessageBuffer++ = i_Data.PayloadLength;

  //========== Payload ==========
  // Payload data
  if (copyPayload)
    memcpy (pMessageBuffer, i_Data.pPayloadBuffer, i_Data.PayloadLength);
  pMessageBuffer += i_Data.PayloadLength;

  //========== Trailer ==========
  memset (pMessageBuffer, 0, trailerSize);

  // Checksum
  switch (m_ChecksumType)
  {
  case UCOP::EChecksumType::None:
    break;
  case UCOP::EChecksumType::CRC8:
    *(pMessageBuffer++) = m_Crc8.maxim (i_pMessageBuffer + 1, headerSize + i_Data.PayloadLength - 1); // header without STX, payload data
    break;
  case UCOP::EChecksumType::CRC16:
    {
      uint16_t crc16 = m_Crc16.modbus (i_pMessageBuffer + 1, headerSize + i_Data.PayloadLength - 1); // header without STX, payload data
      memcpy (pMessageBuffer, &crc16, sizeof (crc16));
      pMessageBuffer += sizeof (crc16);
    }
    break;
  case UCOP::EChecksumType::CRC32:
    {
      uint32_t crc32 = m_Crc32.crc32 (i_pMessageBuffer + 1, headerSize + i_Data.PayloadLength - 1); // header without STX, payload data
      memcpy (pMessageBuffer, &crc32, sizeof (crc32));
      pMessageBuffer += sizeof (crc32);
    }
    break;
  }
  
  // ETX
  *(pMessageBuffer++) = c_MessageEndID;

  o_MessageLength = pMessageBuffer - i_pMessageBuffer;

  return ::EResult::SUCCESS;
}

//--------------------------------------------------------------------
::EResult UCOP::ReadConfigFromEEPROM (uint16_t i_Address)
{
  if (c_EepromConfigTotalSize + i_Address > EEPROM.length ())
    return ::EResult::FAIL_EEPROM_IndexOutsideRange;

  bool                deviceIdsUsed;
  bool                messageIdUsed;
  bool                timestampUsed;
  uint32_t            deviceId;
  UCOP::EChecksumType checksumType;
  uint16_t            configChecksumCRC16;

  EEPROM.get (i_Address + 0, deviceIdsUsed);
  EEPROM.get (i_Address + 1, messageIdUsed);
  EEPROM.get (i_Address + 2, timestampUsed);
  EEPROM.get (i_Address + 3, deviceId);
  EEPROM.get (i_Address + 7, checksumType);
  EEPROM.get (i_Address + 8, configChecksumCRC16);

  uint8_t  byteValue = 0;
  uint16_t checksum = m_Crc16.modbus (&byteValue, 1);
  for (int index = 0; index < c_EepromConfigDataSize; index++)
  {
    byteValue = EEPROM.read (i_Address + index);
    checksum = m_Crc16.modbus_upd (&byteValue, 1);
  }
  if (checksum != configChecksumCRC16)
    return ::EResult::FAIL_Device_ConfigChecksumWrong;

  ::EResult result = CommonConstructor (deviceIdsUsed,
                                        messageIdUsed,
                                        timestampUsed,
                                        deviceId,
                                        checksumType);

  return result;
}
