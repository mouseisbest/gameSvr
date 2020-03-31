#ifndef __SERVER_USER_H__
#define __SERVER_USER_H__


#include <string>
using std::string;


#include "def.pb.h"
using gameSvr::Player;


int server_user_login(string user_name, string password);
Player* server_user_get_by_name(string user_name);

#endif
