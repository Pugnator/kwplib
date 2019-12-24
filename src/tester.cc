#include <message.hpp>
#include <tester.hpp>

using namespace std;

namespace KWP2000
{

kwpClient::kwpClient(uint8_t commport, uint32_t baudrate)
{
  try
  {
    porth = open_port(commport, baudrate);
  }
  catch (const char *e)
  {
    puts(e);
    rethrow_if_nested(e);
  }
}

kwpClient::~kwpClient()
{
  close_port(porth);
}

HANDLE kwpClient::open_port(uint8_t commport, uint32_t baudrate)
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

void kwpClient::close_port(HANDLE commport)
{
  CloseHandle(commport);
}

std::unique_ptr<kwp_message> kwpClient::read_response(kwp_message &request)
{
  DWORD dwEventMask;
  WaitCommEvent(porth, &dwEventMask, NULL);
  uint8_t TempChar;
  uint8_t SerialBuffer[1024] = {0};
  DWORD NoBytesRead;
  unsigned i = 0;
  std::unique_ptr<kwp_message> response(new kwp_message);
  do
  {
    ReadFile(porth, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
    SerialBuffer[i++] = TempChar;
  } while (NoBytesRead > 0);
  i--;

  auto offset = request.length + 4;
  //Skip echo
  response->header.format = SerialBuffer[offset];
  response->header.target = SerialBuffer[offset + 1];
  response->header.source = SerialBuffer[offset + 2];
  auto payld_size = response->header.format & 0x3F;  
  response->header.type = payld_size ? HEADER_SHORT : HEADER_LONG;
  if (response->header.type == HEADER_SHORT)
  {
    response->length = payld_size;
  }
  else
  {
    response->length = SerialBuffer[offset + 3];
  }

  response->add_payload(&SerialBuffer[offset + response->header.type], payld_size);
  response->update_crc();
  uint8_t msg_checksum = SerialBuffer[offset + response->header.type + payld_size];
  response->print();
  return response;
}

void kwpClient::send_message(kwp_message &message)
{
  message.update_header();
  message.update_crc();
  puts("Sending...");
  message.print();
  DWORD bw;
  if (!WriteFile(porth, reinterpret_cast<void *>(message.data), message.length + message.header.type + 1, &bw, NULL))
  {
    throw_with_nested("Writing to port failed with " + GetLastError());
  }
}

const kwp_service *kwpClient::find_ident(const service_alias &name)
{
  for (auto i = 0; service_ids[i].request; ++i)
  {
    if (service_ids[i].short_name == name)
    {
      return &service_ids[i];
    }
  }
  return nullptr;
}
} // namespace KWP2000

int main()
{

  DWORD bw = 0;

  try
  {
    KWP2000::kwpClient tester(8, CBR_38400);
    if (tester.start_communication())
    {
      tester.start_diagnostic_session();
      tester.read_ECUid();
      tester.stop_communication();
    }
  }
  catch (...)
  {
  }
  return 0;
}
