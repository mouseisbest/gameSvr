#ifndef __SERVER_USER_H__
#define __SERVER_USER_H__


#include <string>
#include <map>
using std::string;
using std::map;


#include "def.pb.h"
using gameSvr::Player;

typedef map<string, Player> PLAYER_MAP_TYPE;
typedef PLAYER_MAP_TYPE::reference PLAYER_ITEM_TYPE;


int server_user_login(string user_name, string password);
Player* server_user_get_by_name(string user_name);
Player* server_user_get_by_token(uint64_t token);

void server_user_single_tick(PLAYER_ITEM_TYPE item);


#endif
