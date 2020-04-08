#pragma once
#ifdef WIN32
#include <windows.h>
#endif
#include "message.hpp"
#include "tabs.hpp"
#include <functional>
#include <initializer_list>

const uint16_t KWP_PHY_ADR = 0x6B8F;

using namespace std;

#ifdef WIN32
#include <fmt/format.h>
#include <fmt/locale.h>
#ifdef DEBUG
#define DEBUG_LOG fmt::print
#else
#define DEBUG_LOG(...)
#endif
#else
#define DEBUG_LOG(...)
#endif

namespace KWP2000
{

  typedef enum
  {
    KWP_OK = 0,
    KWP_OPEN_FAILED
  } KWP_ERROR_CODE;

  struct KWPerror : public std::exception
  {
    KWPerror(std::string input, KWP_ERROR_CODE errcode = KWP_OK) : message(input), error(errcode)
    {
    }
    ~KWPerror() throw() {}
    const char *what() const throw()
    {
      return message.c_str();
    }
    std::string message;
    KWP_ERROR_CODE error;
  };

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
    std::unique_ptr<RLI_OLT_tab> read_olt_diag();
    std::unique_ptr<RLI_TRS_tab> read_trs_diag();

    bool read_DTC_by_status();
    ECU_identification_table *read_ECU_identification(const identificationOption &option = ECUIDT);
    bool readMemoryByAddress();
    bool start_communication();
    bool start_diagnostic_session(const uint8_t baudrate);
    bool stop_communication();
    bool stop_diagnostic_session();
    bool tester_present(bool responseRequired);
    bool writeDataByLocalIdentifier();

  private:
#ifdef WIN32
    HANDLE open_port(uint8_t commport, uint32_t baudrate);
    void close_port(HANDLE commport);
#else
    void open_port(uint8_t commport, uint32_t baudrate);
#endif
    void send_message(kwp_message &message);
    std::unique_ptr<kwp_message> read_response(kwp_message &request);

  private:
    const kwp_service *find_service_id(const service_mnemonic &name);
    const kwp_service *find_ident(const uint8_t &code);
#ifdef WIN32
    HANDLE porth;
#endif
    uint8_t portn;
  };

} //namespace KWP2000