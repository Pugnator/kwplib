#include <server.hpp>

namespace KWP2000
{
  kwpServer::kwpServer(uint8_t commport, uint32_t baudrate)
  {
    porth = openPort(commport, baudrate);
    portn = commport;
  }

  kwpServer::~kwpServer()
  {
    close_port(porth);
  }

  uint16_t kwpServer::crc16(std::vector<uint8_t> data)
  {
    uint16_t crc = 0xFFFF;
    for (uint8_t &d : data)
    {
      d = crc >> 8 ^ d;
      d ^= d >> 4;
      crc = (crc << 8) ^ ((uint16_t)(d << 12)) ^ ((uint16_t)(d << 5)) ^ ((uint16_t)d);
    }
    return crc;
  }

  HANDLE kwpServer::openPort(uint8_t commport, uint32_t baudrate)
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

  void kwpServer::close_port(HANDLE commport)
  {
    CloseHandle(commport);
  }

  void kwpServer::handshake()
  {

  }

  void kwpServer::wait()
  {
    //fmt::print("Waiting for incoming data\r\n");

    DWORD dwEventMask;
    WaitCommEvent(porth, &dwEventMask, NULL);
    uint8_t tempChar;
    std::vector<uint8_t> data;
    DWORD NoBytesRead;
    for (;;)
    {
      do
      {
        ReadFile(porth, &tempChar, sizeof(tempChar), &NoBytesRead, NULL);
        if (NoBytesRead)
        {
          data.push_back(tempChar);
        }
      } while (NoBytesRead > 0);
      if(0 == data.size())
      {
        continue;
      }
      fmt::print("Got packet {0} bytes [{1:#04x}]\r\n", data.size(), crc16(data));
      for (const auto &c : data)
      {
        fmt::print("{0:02X} ", c);
      }
      puts("");
      data.clear();
    }
  }

} // namespace KWP2000