
#include <tuple>


#include "server_user.h"
#include "server_object.h"


using gameSvr::def_enum;

uint64_t g_iUserCount;
PLAYER_MAP_TYPE g_userMap;

static void server_user_init_data(Player *player)
{
    if (NULL == player)
    {
        return;
    }

    memset(player, 0, sizeof(*player));
    player->set_token(g_iUserCount++);
    player->set_mutex((int64_t)new std::mutex);
    player->set_objid(server_object_create(ObjType::OBJ_TYPE_TANK, player->token(), 0));
}

static void server_user_clear_data(Player *player)
{
    if (NULL == player)
    {
        return;
    }
    delete (std::mutex*)player->mutex();
}

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
    server_user_init_data(&player); 
    player.mutable_baseinfo()->set_username(user_name);
    player.set_token(++g_iUserCount);
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


Player* server_user_get_by_token(uint64_t token)
{
    for (PLAYER_MAP_TYPE::iterator it = g_userMap.begin(); 
        it != g_userMap.end(); ++it)
    {
        if (it->second.token() == token)
        {
            return &it->second;
        }
    }
    return nullptr;
}

void server_user_single_tick(PLAYER_ITEM_TYPE item)
{

}
