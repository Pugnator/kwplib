#include "client.hpp"
#include <functional>

int main()
{
  try
  {
    auto tester = std::make_unique<KWP2000::kwpClient>(6, 10400);    

    uint8_t retry = 10;

    if(!tester->start_communication() && retry--)
    {
      Sleep(100);
    }

    if (!retry)
    {
      return 1;
    }

    if (!tester->start_diagnostic_session(KWP2000::ENHANCED_BAUDRATE))
      return 1;

    if (!tester->tester_present(true))
      return 1;

    KWP2000::ECU_identification_table *id = tester->read_ECU_identification();
    if (id)
    {
      printf("\r\nvehicleManufacturerECUHardwareNumber: %.*s\r\n", sizeof(id->vehicleManufacturerECUHardwareNumber), id->vehicleManufacturerECUHardwareNumber);
    }

    tester->read_DTC_by_status();

    //trial(std::function<std::unique_ptr<KWP2000::RLI_ASS_tab>()>(tester->read_rli_ass));

    std::unique_ptr<KWP2000::RLI_ASS_tab> rli = tester->read_rli_ass();
    printf("Battery: %f\r\n", rli->get_battery());
    printf("AFR: %f\r\n", rli->get_afr());
    printf("Correction: %f\r\n", rli->get_correction());    

    tester->stop_diagnostic_session();
    tester->stop_communication();
  }
  catch (...)
  {
  }  
}