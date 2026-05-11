#include <Arduino.h>
#include <Streaming.h>
#include <UCOP.h>

UCOP* m_pUCOP;

uint8_t m_PayloadSendBuffer[20];
uint8_t m_SendBuffer       [80];
uint8_t m_ReceiveBuffer    [80];
uint8_t m_PayloadRecvBuffer[20];

uint16_t messageLength = 0;

FastCRC16 m_Crc16;

void setup ()
{
  memset (m_PayloadSendBuffer, 0xFF, sizeof (m_PayloadSendBuffer));
  memset (m_SendBuffer,        0xFF, sizeof (m_SendBuffer));
  memset (m_ReceiveBuffer,     0xFF, sizeof (m_ReceiveBuffer));
  memset (m_PayloadRecvBuffer, 0xFF, sizeof (m_PayloadRecvBuffer));

  EResult result;
  byte payloadLength = 6;
  char text[10] = "Hello!";
  byte textLength = 10;
  memcpy (&m_PayloadSendBuffer, &text, textLength);

  Serial.begin (38400);
  delay (2000);

  Serial << "SendBuffer Addr=" << _HEX4((uint16_t)m_SendBuffer)    << " Len=" << sizeof (m_SendBuffer)    << endl;
  Memory_PrintLn (m_SendBuffer, sizeof (m_SendBuffer));
  Serial << "RecvBuffer Addr=" << _HEX4((uint16_t)m_ReceiveBuffer) << " Len=" << sizeof (m_ReceiveBuffer) << endl;
  Memory_PrintLn (m_ReceiveBuffer, sizeof (m_ReceiveBuffer));

  m_pUCOP = new UCOP (true, true, false, 101, EUcopChecksumType::CRC16, result);
  Serial << "UCOP ctor Result: " << (int)result << " = " << UCOP::GetResultText (result) << endl;

  uint16_t messageLength = 0;
  UcopData stucData = UcopData (EUcopAction::Read, 258, 42);
  stucData.SetPayloadInfo (m_PayloadSendBuffer, sizeof (m_PayloadSendBuffer), payloadLength);
  result = m_pUCOP->ComposeRequest (stucData, m_SendBuffer, sizeof (m_SendBuffer), messageLength);
  Serial << "ComposeRequest() Result: " << (int)result << " = " << UCOP::GetResultText (result) << ", MsgLen=" << messageLength << endl;
  Memory_PrintLn (m_SendBuffer, sizeof (m_SendBuffer));

  Serial.println ("create data for receiver...");
  memcpy (m_ReceiveBuffer + 0,  m_SendBuffer + 0,  3);
  memcpy (m_ReceiveBuffer + 7,  m_SendBuffer + 3,  4);
  memcpy (m_ReceiveBuffer + 3,  m_SendBuffer + 7,  4);
  memcpy (m_ReceiveBuffer + 11, m_SendBuffer + 11, messageLength - 11);
  Memory_PrintLn (m_ReceiveBuffer, sizeof (m_ReceiveBuffer));

  Serial.println ("Recalc checksum...");
  uint16_t checksum = m_Crc16.modbus (&m_ReceiveBuffer[1], messageLength - 1 - 2 - 1);
  memcpy (&m_ReceiveBuffer[messageLength - 1 - 2], &checksum, 2);
  Memory_PrintLn (m_ReceiveBuffer, sizeof (m_ReceiveBuffer));

  Serial.println ("move and duplicate...");
  memmove (m_ReceiveBuffer + 20, m_ReceiveBuffer, messageLength);
  memset (m_ReceiveBuffer, 0xFF, 20);
  byte msgWrap = 15;
  memcpy (m_ReceiveBuffer + sizeof (m_ReceiveBuffer) - msgWrap, m_ReceiveBuffer + 20, msgWrap);
  memcpy (m_ReceiveBuffer, m_ReceiveBuffer + 35, messageLength - msgWrap);
  Memory_PrintLn (m_ReceiveBuffer, sizeof (m_ReceiveBuffer));

  UcopData          recvData;
  recvData.SetPayloadInfo (m_PayloadRecvBuffer, sizeof (m_PayloadRecvBuffer));
  EUcopMessageType  recvMessageType   = EUcopMessageType::None;
  uint8_t           recvMessageLength = 0;

  uint16_t bufferStartIndex = 0;
  do
  {
    result = m_pUCOP->AnalyseMessage (m_ReceiveBuffer,
                                      sizeof (m_ReceiveBuffer),
                                      bufferStartIndex,
                                      recvData,
                                      recvMessageType,
                                      recvMessageLength);
    Serial << "AnalyseMessage() Result: " << (int)result << " = " << UCOP::GetResultText (result) << endl;
    Serial << F("Message Type:        ") << (uint8_t)recvMessageType        << endl;
    Serial << F("Action:              ") << (uint8_t)recvData.Action        << endl;
    Serial << F("Remote Device Id:    ") << recvData.RemoteDeviceId         << endl;
    Serial << F("Message Id:          ") << recvData.MessageId              << endl;
    Serial << F("Timestamp:           ") << recvData.Timestamp              << endl;
    Serial << F("CommandId:           ") << recvData.CommandId              << endl;
    Serial << F("Result:              ") << (uint8_t)recvData.MessageResult << endl;
    Serial << F("Payload Data Length: ") << recvData.PayloadLength          << endl;
    Serial << F("Payload Data: ");
    Memory_PrintLn (m_PayloadRecvBuffer, recvData.PayloadLength);
    Serial << F("Message Length: ") << recvMessageLength      << endl;

    Memory_PrintLn (m_ReceiveBuffer, sizeof (m_ReceiveBuffer));
  }
  while (result == ::EResult::SUCCESS);
}

void loop ()
{
}
