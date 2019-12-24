#include <message.hpp>
#include <tester.hpp>

namespace KWP2000
{

bool kwpTester::start_communication()
{
  const kwp_identificator *rq = find_ident(KWP_STC);
  if (!rq)
  {
    return false;
  }
  kwp_message message;
  message.add_ident(*rq);
  send_message(message);
  auto resp = read_response(message);
  const kwp_identificator *rp = resp->get_ident();
  return rp && rp->short_name == KWP_STC;
}

bool kwpTester::stop_communication()
{
  const kwp_identificator *rq = find_ident(KWP_SPC);
  if (!rq)
  {
    return false;
  }
  kwp_message message;
  message.add_ident(*rq);
  send_message(message);
  auto resp = read_response(message);
  const kwp_identificator *rp = resp->get_ident();
  return rp && rp->short_name == KWP_STC;
}
} // namespace KWP2000