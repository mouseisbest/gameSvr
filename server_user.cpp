
#include <map>
#include <tuple>


#include "server_user.h"


using std::map;

uint64_t g_iUserCount;
map<string, Player> g_userList;

int server_user_login(string user_name, string password)
{
    if (!user_name.length() /*|| !password.length()*/)
    {
        return -1;
    }
    auto user = g_userList.find(user_name);
    if (user != g_userList.end())
    {
        return 0;
    }
    // 创建用户数据
    Player player;
    player.mutable_baseinfo()->set_username(user_name);
    player.set_token(g_iUserCount++);
    g_userList.insert(std::make_pair(user_name, player));
    return 0;
}

Player* server_user_get_by_name(string user_name)
{
    auto user = g_userList.find(user_name);
    if (user == g_userList.end())
    {
        return NULL;
    }
    return &user->second;
}
