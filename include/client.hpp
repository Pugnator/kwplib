#pragma once
#include <windows.h>
#include <message.hpp>
#include <initializer_list>

const uint16_t KWP_PHY_ADR = 0x6B8F;

namespace KWP2000
{

class kwpClient
{
public:
  kwpClient(uint8_t commport, uint32_t baudrate);
  ~kwpClient();

public:
  std::unique_ptr<kwp_message> process_command(const service_mnemonic &cmd, const std::initializer_list<uint8_t> &params = {});
  bool clear_diagnostic_information(bool clear_all = true);
  bool ECU_reset();
  bool inputOutputControlByLocalIdentifier();
  std::unique_ptr<RLI_ASS_tab> read_rli_ass();
  bool read_DTC_by_status();
  ECU_identification_table *read_ECU_identification(const identificationOption &option = ECUIDT);
  bool readMemoryByAddress();
  bool start_communication();
  bool start_diagnostic_session();
  bool stop_communication();
  bool stop_diagnostic_session();
  bool tester_present(bool responseRequired);
  bool writeDataByLocalIdentifier();

private:
  HANDLE open_port(uint8_t commport, uint32_t baudrate);
  void close_port(HANDLE commport);
  void send_message(kwp_message &message);
  std::unique_ptr<kwp_message> read_response(kwp_message &request);

private:
  const kwp_service *find_service_id(const service_mnemonic &name);
  const kwp_service *find_ident(const uint8_t &code);
  HANDLE porth;
};

} //namespace KWP2000