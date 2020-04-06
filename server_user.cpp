
#include <tuple>


#include "server_user.h"


using gameSvr::def_enum;

uint64_t g_iUserCount;
PLAYER_MAP_TYPE g_userMap;

int server_user_login(string user_name, string password)
{
    if (!user_name.length() /*|| !password.length()*/)
    {
        return -1;
    }
    if (g_userMap.size() > def_enum::MAX_PLAYER_NUM)
    {
        return -1;
    }
    auto user = g_userMap.find(user_name);
    if (user != g_userMap.end())
    {
        return 0;
    }
    // 创建用户数据
    Player player;
    player.mutable_baseinfo()->set_username(user_name);
    player.set_token(g_iUserCount++);
    g_userMap.insert(std::make_pair(user_name, player));
    return 0;
}

Player* server_user_get_by_name(string user_name)
{
    auto user = g_userMap.find(user_name);
    if (user == g_userMap.end())
    {
        return NULL;
    }
    return &user->second;
}



void server_user_single_tick(PLAYER_ITEM_TYPE item)
{

}
