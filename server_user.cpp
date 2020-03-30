
#include <map>
#include <tuple>


#include "server_user.h"


using std::map;

map<string, Role> g_userList;

int server_user_login(string user_name, string password)
{
    auto user = g_userList.at(user_name);
    if (user.has_baseinfo())
    {
        return 0;
    }
    // 创建用户数据
    Role role;
    role.mutable_baseinfo()->set_username(user_name);
    g_userList.insert(std::make_pair(user_name, role));
    return 0;
}

Role server_user_get_by_name(string user_name)
{
    return g_userList.at(user_name);
}
