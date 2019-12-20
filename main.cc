#include <windows.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <string>
#include <kwp.hpp>

using namespace std;

namespace KWP2000
{

const kwp_ident kwp_table[] =
    {
        {.request = 0x81, .reply = 0xC1, .inter_name = "startCommunication", .short_name = KWP_STC},
        {.request = 0x82, .reply = 0xC2, .inter_name = "stopCommunication", .short_name = KWP_SPC},
        {.request = 0x10, .reply = 0x50, .inter_name = "startDiagnosticSession", .short_name = KWP_STDS},
        {.request = 0x20, .reply = 0x60, .inter_name = "stopDiagnosticSession", .short_name = KWP_SPDS},
        {.request = 0x11, .reply = 0x51, .inter_name = "ecuReset", .short_name = KWP_ER},
        {.request = 0x14, .reply = 0x54, .inter_name = "clearDiagnosticInformation", .short_name = KWP_CDI},
        {.request = 0x18, .reply = 0x58, .inter_name = "readDiagnosticTroubleCodesByStatus", .short_name = KWP_RDTCBS},
        {.request = 0x1A, .reply = 0x5A, .inter_name = "readEcuIdentification", .short_name = KWP_REI},
        {.request = 0x21, .reply = 0x61, .inter_name = "readDataByLocalIdentifier", .short_name = KWP_RDBLI},
        {.request = 0x23, .reply = 0x63, .inter_name = "readMemoryByAddress", .short_name = KWP_RMBA},
        {.request = 0x30, .reply = 0x70, .inter_name = "inputOutputControlByLocalIdentifier", .short_name = KWP_IOCBLI},
        {.request = 0x3B, .reply = 0x7B, .inter_name = "writeDataByLocalIdentifier", .short_name = KWP_WDBLI},
        {.request = 0x3E, .reply = 0x7E, .inter_name = "testerPresent", .short_name = KWP_TP},
        {0, 0}};


const kwp_reply kwp_reply_table[]=
{
  {.reply = 0x10, .inter_name="generalReject", .short_name=GR},
  {0, 0}
};

kwpTester::kwpTester(uint8_t commport, uint32_t baudrate)
{
  try
  {
    port = open_port(commport, baudrate);
  }
  catch (const char *e)
  {
    puts(e);
    rethrow_if_nested(e);
  }
}

kwpTester::~kwpTester()
{
  close_port(port);
}

HANDLE kwpTester::open_port(uint8_t commport, uint32_t baudrate)
{
  std::string device_path = "\\\\.\\COM" + std::to_string(commport);
  HANDLE port_handle = CreateFile(device_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

  if (port_handle == INVALID_HANDLE_VALUE)
  {
    throw_with_nested("Error opening serial port");
  }

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  GetCommState(port_handle, &dcbSerialParams);
  dcbSerialParams.BaudRate = baudrate;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  SetCommState(port_handle, &dcbSerialParams);

  SetCommMask(port_handle, EV_RXCHAR);
  return port_handle;
}

void kwpTester::close_port(HANDLE commport)
{
  CloseHandle(commport);
}

kwp_message kwpTester::read_response()
{
  DWORD dwEventMask;
  WaitCommEvent(port, &dwEventMask, NULL);
  uint8_t TempChar;              //Temporary character used for reading
  uint8_t SerialBuffer[256 + 5]; //Buffer for storing Rxed Data
  DWORD NoBytesRead;
  int i = 0;
  kwp_message response;
  do
  {
    ReadFile(port, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
    SerialBuffer[i] = TempChar; // Store Tempchar into buffer
    i++;
  } while (NoBytesRead > 0);
  return response;
}

void kwpTester::send_message(kwp_message &message)
{
  message.update_header();
  message.update_crc();
  printf("Message header: 0x%X 0x%X 0x%X\r\n", message.data[0], message.data[1], message.data[2]);
  printf("Sending message of type 0x%X, length %u [CRC: 0x%X]\r\n", message.data[message.header.type], message.header.fmt, message.data[message.header.type + message.header.fmt]);
  DWORD bw;
  if (!WriteFile(port, reinterpret_cast<void *>(message.data), message.length + message.header.type + 1, &bw, NULL))
  {
    throw_with_nested("Writing to port failed with " + GetLastError());
  }
}

const kwp_ident *kwpTester::find_ident(const kwp_short_name &name)
{
  for (auto i = 0; kwp_table[i].request; ++i)
  {
    if (kwp_table[i].short_name == name)
    {
      return &kwp_table[i];
    }
  }
  return nullptr;
}

void kwp_message::add_ident(const kwp_ident &id)
{
  data[header.type] = id.request;
  length++;
}

void kwp_message::update_format()
{
  header.fmt |= 1 << 7;
  header.fmt |= length & 0b00111111;
}

void kwp_message::update_crc()
{
  checksum = header.get_sum();
  for (auto i = 0; i < length; ++i)
  {
    checksum += data[header.type + i];
  }
  data[header.type + length] = checksum;
}

void kwp_message::update_header()
{
  update_format();
  data[0] = header.fmt;
  data[1] = header.tgt;
  data[2] = header.src;
  if (header.type == HEADER_TYPE_4)
  {
    data[3] = length;
  }
}

uint8_t kwp_header::get_sum()
{
  return fmt + tgt + src + len;
}

bool kwpTester::start()
{
  kwp_message message;
  const kwp_ident *rq = find_ident(KWP_STC);
  if (rq)
  {
    message.add_ident(*rq);
    send_message(message);
    read_response();
  }
  return true;
}
} // namespace KWP2000

int main()
{

  DWORD bw = 0;

  try
  {
    KWP2000::kwpTester tester(8, CBR_38400);
    tester.start();
  }
  catch (...)
  {
  }
  return 0;
}
