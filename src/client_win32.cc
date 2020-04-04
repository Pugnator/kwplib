#include <message.hpp>
#include <client.hpp>
#include <exception>

namespace KWP2000
{

kwpClient::kwpClient(uint8_t commport, uint32_t baudrate)
{
  try
  {
    porth = open_port(commport, baudrate);
    portn = commport;
  }
  catch (const char *e)
  {
    DEBUG_LOG(e);
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

  COMMTIMEOUTS timeouts;
  if ((GetCommTimeouts(port_handle, &timeouts) == 0))
  {
    throw_with_nested("error getting timeouts");
  }
  timeouts.ReadIntervalTimeout = 10;
  timeouts.ReadTotalTimeoutMultiplier = 1;
  timeouts.ReadTotalTimeoutConstant = 500;
  timeouts.WriteTotalTimeoutMultiplier = 1;
  timeouts.WriteTotalTimeoutConstant = 500;
  if (SetCommTimeouts(port_handle, &timeouts) == 0)
  {
    throw_with_nested("error setting timeouts");
  }

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
  uint8_t tempChar;
  uint8_t readbuf[512] = {0};
  DWORD NoBytesRead;
  size_t bytes_total = 0;
  std::unique_ptr<kwp_message> response(new kwp_message);
  do
  {
    ReadFile(porth, &tempChar, sizeof(tempChar), &NoBytesRead, NULL);
    if (NoBytesRead)
    {
      readbuf[bytes_total++] = tempChar;
    }
  } while (NoBytesRead > 0);

  auto offset = request.length + 4;
  //Skip echo
  if (offset == bytes_total)
  {
    //we have only echo
    return nullptr;
  }
  response->header.format = readbuf[offset];
  response->header.target = readbuf[offset + 1];
  response->header.source = readbuf[offset + 2];
  auto short_size = response->header.format & 0x3F;

  auto actual_data_size = bytes_total - offset - 4;
  DEBUG_LOG("Total: %u, Payload: %u\r\n", actual_data_size, short_size);

  if (0x80 == response->header.format || actual_data_size > short_size) //XXX:
  {
    response->header.type = HEADER_LONG;
    response->length = readbuf[offset + 3];
  }
  else
  {
    response->header.type = HEADER_SHORT;
    response->length = short_size;
  }

  response->add_payload(&readbuf[offset + response->header.type], response->length);
  uint8_t msg_checksum = readbuf[offset + response->header.type + response->length];
  response->data[response->header.type + response->length] = msg_checksum;
  response->checksum = msg_checksum;
  DEBUG_LOG("Receiving...");
  response->print();
  return response;
}

void kwpClient::send_message(kwp_message &message)
{
  message.update_header();
  message.update_crc();
  DEBUG_LOG("Sending...");
  message.print();
  DWORD bw;
  if (!WriteFile(porth, reinterpret_cast<void *>(message.data), message.length + message.header.type + 1, &bw, NULL))
  {
    throw_with_nested("Writing to port failed with " + GetLastError());
  }
}

} // namespace KWP2000