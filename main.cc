#include <windows.h>
#include <message.hpp>
#include <tester.hpp>

using namespace std;

namespace KWP2000
{

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

kwp_message kwpTester::read_response(kwp_message &request)
{
  DWORD dwEventMask;
  WaitCommEvent(port, &dwEventMask, NULL);
  uint8_t TempChar;
  uint8_t SerialBuffer[1024] = {0};
  DWORD NoBytesRead;
  unsigned i = 0;
  kwp_message response;
  do
  {
    ReadFile(port, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
    SerialBuffer[i++] = TempChar;
  } while (NoBytesRead > 0);
  i--;

  auto offset = request.length + 4;
  //Skip echo
  response.header.fmt = SerialBuffer[offset];
  response.header.tgt = SerialBuffer[offset + 1];
  response.header.src = SerialBuffer[offset + 2];

  auto payld_size = response.header.fmt & 0x3F;
  response.header.type = payld_size ? HEADER_TYPE_3 : HEADER_TYPE_4;
  if (response.header.type == HEADER_TYPE_3)
  {
    response.length = payld_size;
  }
  else
  {
    response.length = SerialBuffer[offset + 3];
  }

  response.add_payload(&SerialBuffer[offset + response.header.type], payld_size);
  response.update_crc();
  uint8_t msg_checksum = SerialBuffer[offset + response.header.type + payld_size];
  response.print();
  return response;
}

void kwpTester::send_message(kwp_message &message)
{
  message.update_header();
  message.update_crc();
  puts("Sending...");
  message.print();
  DWORD bw;
  if (!WriteFile(port, reinterpret_cast<void *>(message.data), message.length + message.header.type + 1, &bw, NULL))
  {
    throw_with_nested("Writing to port failed with " + GetLastError());
  }
}

const kwp_identificator *kwpTester::find_ident(const kwp_short_name &name)
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

bool kwpTester::start_communication()
{
  kwp_message message;
  const kwp_identificator *rq = find_ident(KWP_STC);
  if (rq)
  {
    message.add_ident(*rq);
    send_message(message);
    auto resp = read_response(message);
    const kwp_identificator *rp = resp.get_ident();
    return rp && rp->short_name == KWP_STC;
  }
  return true;
}
} // namespace KWP2000

int main()
{

  DWORD bw = 0;

  try
  {
    KWP2000::kwpTester tester(9, CBR_38400);
    if (tester.start_communication())
    {
      puts("Comm started");
    }
  }
  catch (...)
  {
  }
  return 0;
}
