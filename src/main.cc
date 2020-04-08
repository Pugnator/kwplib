#include "client.hpp"
#include <getopt.h>
#include <functional>

using namespace KWP2000;
using namespace std;

int main(int argc, char **argv)
{
  puts("Started");
  int opt = 0;
  int verbose = 0;
  int option_index = 0;
  char *port = NULL;
  struct option long_options[] =
      {
          {"verbose", no_argument, &verbose, 0},
          {"port", required_argument, 0, 'p'},
          {NULL, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "p:hv", long_options, &option_index)) != -1)
  {
    switch (opt)
    {
    case 'v':
      verbose = 1;
      break;
    case 'p':
      port = optarg;
      break;
    case 'h':
    case '?':
      return 0;
      break;
    }
  }

  try
  {
    auto tester = make_unique<kwpClient>(5, 57600);

    uint8_t retry = 10;

    if (!tester->start_communication() && retry--)
    {
      Sleep(50);
    }

    if (!retry)
    {
      return 1;
    }

    if (!tester->start_diagnostic_session(ENHANCED_BAUDRATE))
    {
      DEBUG_LOG("Start diagnostic session failed");
      return 1;
    }

    if (!tester->tester_present(true))
    {
      DEBUG_LOG("Tester present failed");
      return 1;
    }

    ECU_identification_table *id = tester->read_ECU_identification();
    if (id)
    {
      DEBUG_LOG("\r\nvehicleManufacturerECUHardwareNumber: %.*s\r\n", sizeof(id->vehicleManufacturerECUHardwareNumber), id->vehicleManufacturerECUHardwareNumber);
    }
    else
    {
      DEBUG_LOG("Failed to get ID\r\n");
    }

    tester->read_DTC_by_status();

#if 1
    unique_ptr<RLI_OLT_tab> rli = tester->read_olt_diag();
    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
    {       
      fmt::print("RPM: {0}\r\n", rli->get_rpm());
      fmt::print("THR: {0}\r\n", rli->get_throttle());
      fmt::print("ADV: {0}\r\n", rli->get_advance());
      fmt::print("Water: {0}\r\n", rli->get_water_temp());
      fmt::print("Air temp: {0}\r\n", rli->get_air_temp());
      fmt::print("AFR: {0:4}\r\n", rli->get_afr());
      fmt::print("Battery: {0:2}V\r\n", rli->get_battery());
      rli = tester->read_olt_diag();
    }
#endif
    tester->stop_diagnostic_session();
    tester->stop_communication();
  }
  catch (const KWPerror &e)
  {
    DEBUG_LOG("Error {0}\r\n", e.what());
  }
}