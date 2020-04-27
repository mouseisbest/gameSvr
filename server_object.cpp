#include <algorithm>
#include <thread>

#include "server_object.h"
#include "server_cs_processor.h"
#include "time_tool.h"
#include "def.pb.h"
#include "cs.pb.h"
#include "res.pb.h"

using std::for_each;
using std::make_pair;
using gameSvr::ObjType;
using gameSvr::Tank;
using gameSvr::def_enum;
using gameSvr::res_enum;
using gameSvr::CmdID;
using gameSvr::CSObject;

int g_iObjectNum;
OBJECT_MAP_TYPE g_objectMap;
std::mutex g_objectMutex;
extern GameServerImpl g_networkService;


void server_object_position_change(Object &obj, Direction dir)
{
    auto pos = obj.mutable_position();
    auto info = obj.mutable_tank()->mutable_battleinfo();

    int x = pos->mutable_pos()->x();
    int y = pos->mutable_pos()->y();
    int speed = info->speed();
    // 更新位置
    switch (dir)
    {
    case Direction::DIRECTION_EAST:
        {
            x += speed;
            break;
        }
    case Direction::DIRECTION_WEST:
        {
            x -= speed;
            break;
        }
    case Direction::DIRECTION_SOUTH:
        {
            y += speed;
            break;
        }
    case Direction::DIRECTION_NORTH:
        {
            y -= speed;
            break;
        }
    default:
        break;
    }

    // 处理超过边界的情况
    if (x < 0 || x >= res_enum::MAX_MAP_WIDTH || 
        y < 0 || y >= res_enum::MAX_MAP_HEIGHT)
    {
        obj.set_collision(1);
    }
    if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (x >= res_enum::MAX_MAP_WIDTH)
    {
        x = res_enum::MAX_MAP_WIDTH;
    }
    if (y >= res_enum::MAX_MAP_HEIGHT)
    {
        y = res_enum::MAX_MAP_HEIGHT;
    }
    pos->mutable_pos()->set_x(x);
    pos->mutable_pos()->set_y(y);
}


void server_object_tick_bullet(Object &obj)
{
    server_object_position_change(obj, obj.mutable_position()->dir());
}


void server_object_move_tick(OBJECT_ITEM_TYPE item)
{
    Object *object = &item.second;
    switch (object->type())
    {
    case ObjType::OBJ_TYPE_TANK:
        {
            break;
        }
    case ObjType::OBJ_TYPE_BULLET:
        {
            server_object_tick_bullet(*object);
            break;
        }
    default:
        break;
    }

}

void server_object_combat_tick(OBJECT_ITEM_TYPE item)
{
    Object *object = &item.second;
    switch (object->type())
    {
    case ObjType::OBJ_TYPE_TANK:
        {
            break;
        }
    case ObjType::OBJ_TYPE_BULLET:
        {
            break;
        }
    default:
        break;
    }

}


static void server_object_init_tank(Object &object)
{
    auto info = object.mutable_tank()->mutable_battleinfo();
    info->set_hp(300);
    info->set_speed(2);
    info->set_damage(10);
    info->set_defense(5);
    info->set_lastfiretime(0);
    object.mutable_position()->mutable_pos()->set_x(5);
    object.mutable_position()->mutable_pos()->set_y(5);
}


static int server_object_create_bullet_check(Object *parent, time_t time)
{
    if (nullptr == parent)
    {
        return -1;
    }

    auto tank = parent->mutable_tank();
    if (tank->mutable_battleinfo()->lastfiretime() - get_time() < def_enum::MIN_TANK_FIRE_TIME)
    {
        return -1;
    }


    return 0;
}

uint64_t server_object_create(ObjType objType, uint64_t param1, uint64_t param2)
{
    Object object;
    switch (objType)
    {
    case ObjType::OBJ_TYPE_TANK:
        {
            object.set_type(ObjType::OBJ_TYPE_TANK);
            object.mutable_tank()->set_userid(param1);
            server_object_init_tank(object);
            break;
        }
    case ObjType::OBJ_TYPE_BULLET:
        {
            if (0 == param1)
            {
                return -1;
            }
            Object *parent = (Object*)param1;
            time_t now = get_time();
            int iRet = server_object_create_bullet_check(&object, now);
            if (iRet)
            {
                return iRet;
            }
            auto tank = parent->mutable_tank();
            tank->mutable_battleinfo()->set_lastfiretime(now);
            object.set_type(ObjType::OBJ_TYPE_BULLET);
            object.mutable_position()->mutable_pos()->set_x(parent->mutable_position()->mutable_pos()->x());
            object.mutable_position()->mutable_pos()->set_y(parent->mutable_position()->mutable_pos()->y());
            object.mutable_position()->set_dir(parent->mutable_position()->dir());
            object.mutable_bullet()->set_speed(def_enum::DEFAULT_BULLET_SPEED);

            break;
        }
    default:
        break;
    }
    std::unique_lock<std::mutex> lock(g_objectMutex);
    object.set_objid(g_iObjectNum++);
    g_objectMap.insert(make_pair(object.objid(), object));
    return object.objid();
}

Object *server_object_find(uint64_t objid)
{
    std::unique_lock<std::mutex> lock(g_objectMutex);
    auto res = g_objectMap.find(objid);
    if (res == g_objectMap.end())
    {
        return nullptr;
    }
    return &res->second;
}

static void server_object_to_cs_object(Object &obj, CSObject &csObj)
{
    csObj.set_type(obj.type());
    csObj.set_objid(obj.objid());
    csObj.mutable_position()->CopyFrom(*obj.mutable_position());
    switch (obj.type())
    {
    case ObjType::OBJ_TYPE_TANK:
        {
            auto battle_info = csObj.mutable_tank()->mutable_info();
            battle_info->set_hp(obj.mutable_tank()->mutable_battleinfo()->hp());
            battle_info->set_dir(obj.mutable_position()->dir());
            break;
        }
    case ObjType::OBJ_TYPE_BULLET:
        {
            csObj.mutable_bullet()->set_dir(obj.mutable_position()->dir());
            break;
        }
    default:
        break;
    }
}

static void server_object_broadcast()
{
    if (g_objectMap.size() == 0)
    {
        return;
    }
    CSMessageS msg;
    msg.set_cmd(CmdID::CS_CMD_MAP_INFO);
    auto array = msg.mutable_mapinfo();
    for (OBJECT_MAP_TYPE::iterator it = g_objectMap.begin(); it != g_objectMap.end(); ++it)
    {
        auto csObj = array->add_object();
        server_object_to_cs_object(it->second, *csObj);
    }
    printf("%s: %d objects packed\n", __FUNCTION__, array->object_size());
    g_networkService.BroadcastMsg(msg);
}

void server_object_tick()
{
    std::unique_lock<std::mutex> lock(g_objectMutex);
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_move_tick);
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_combat_tick);
    server_object_broadcast();
}


int server_object_remove(uint64_t objId)
{
    std::unique_lock<std::mutex> lock(g_objectMutex);

    OBJECT_MAP_TYPE::iterator it = g_objectMap.find(objId);
    if (it != g_objectMap.end())
    {
        g_objectMap.erase(it);
    }
    return 0;
}
