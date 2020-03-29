
#include <map>

#include "def.pb.h"

#include "server_user.h"


using std::map;
using gameSvr::Role;

map<uint64_t, Role> g_userList;

int server_user_login(string user_name, string password)
{

    return 0;
}
