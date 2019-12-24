#include <message.hpp>
#include <tester.hpp>

namespace KWP2000
{

std::unique_ptr<kwp_message> kwpClient::process_command(const service_alias &cmd, const std::initializer_list<uint8_t> &params)
{
  const kwp_service *rq = find_ident(cmd);
  if (!rq)
  {
    return nullptr;
  }
  kwp_message message;
  message.add_ident(*rq);
  for (auto p : params)
  {
    message.add_param(p);
  }
  send_message(message);
  return read_response(message);
}

bool kwpClient::start_communication()
{
  auto resp = process_command(KWP_STC, {});
  auto *rp = resp->get_ident();
  if (rp->bad())
  {
    return false;
  }
  uint16_t key = (resp->data[resp->header.type] << 8) | resp->data[resp->header.type + 1];
  if (KWP_KEYWORD == key)
  {
    //
  }
  return rp && rp->short_name == KWP_STC;
}

bool kwpClient::stop_communication()
{
  auto resp = process_command(KWP_SPC, {});
  const kwp_service *rp = resp->get_ident();
  if (rp->bad())
  {
    return false;
  }
  return rp && rp->short_name == KWP_SPC;
}

bool kwpClient::start_diagnostic_session()
{
  auto resp = process_command(KWP_STDS, {0x81, 0x26});
  const kwp_service *rp = resp->get_ident();
  if (rp->bad())
  {
    return false;
  }
  return true;
}

bool kwpClient::read_ECUid()
{
  auto resp = process_command(KWP_REI, {0x80});
  const kwp_service *rp = resp->get_ident();
  if (rp->bad())
  {
    return false;
  }
  return true;
}
} // namespace KWP2000