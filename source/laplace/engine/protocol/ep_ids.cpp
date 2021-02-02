#include "ids.h"

using namespace laplace;
using namespace engine;
using namespace protocol;
using namespace std;

const string ids::table[] = { "", "publickey", "requestevents",
  "clientping", "serverlaunch", "serveraction", "serverpause",
  "serverclock", "serverseed", "serverquit", "cliententer",
  "clientleave", "clientready", "debug", "slotcreate",
  "slotremove" };
