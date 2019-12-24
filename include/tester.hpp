#pragma once
#include <windows.h>
#include <message.hpp>
#include <initializer_list>

const uint16_t KWP_KEYWORD = 0x6B8F;

namespace KWP2000
{

class kwpClient
{
public:
  kwpClient(uint8_t commport, uint32_t baudrate);
  ~kwpClient();

public:  
  std::unique_ptr<kwp_message> process_command(const service_alias& cmd, const std::initializer_list<uint8_t>& params);
  bool start_communication();
  bool stop_communication();
  bool start_diagnostic_session();
  bool stop_diagnostic_session();
  bool read_ECUid();

private:
  HANDLE open_port(uint8_t commport, uint32_t baudrate);
  void close_port(HANDLE commport);
  void send_message(kwp_message &message);
  std::unique_ptr<kwp_message> read_response(kwp_message &request);

private:
  const kwp_service *find_ident(const service_alias &name);
  const kwp_service *find_ident(const uint8_t &code);
  HANDLE porth;
};

} //namespace KWP2000