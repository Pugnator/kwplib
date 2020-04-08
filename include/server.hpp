#pragma once
#include <getopt.h>
#include <functional>
#include <windows.h>
#include "message.hpp"
#include <initializer_list>

#include <fmt/format.h>
#include <fmt/locale.h>

using namespace std;

#ifdef DEBUG
#define DEBUG_LOG fmt::print
#else
#define DEBUG_LOG(...)
#endif

namespace KWP2000
{
  class kwpServer
  {
  public:
    kwpServer(uint8_t commport, uint32_t baudrate);
    ~kwpServer();

  public:
    void wait();
    void handshake();
    uint16_t crc16(std::vector<uint8_t> data);
  private:
    HANDLE openPort(uint8_t commport, uint32_t baudrate);
    void close_port(HANDLE commport);
    void send_message(kwp_message &message);
    HANDLE porth;
    uint8_t portn;
  };
} // namespace KWP2000