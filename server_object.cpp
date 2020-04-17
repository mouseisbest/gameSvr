#include <algorithm>

#include "server_object.h"
#include "time_tool.h"
#include "def.pb.h"
#include "res.pb.h"

using std::for_each;
using std::make_pair;
using gameSvr::ObjType;
using gameSvr::Tank;
using gameSvr::def_enum;
using gameSvr::res_enum;

int g_iObjectNum;
OBJECT_MAP_TYPE g_objectMap;

void server_object_single_tick(OBJECT_ITEM_TYPE item)
{
    Object *object = &item.second;
    switch (object->type())
    {
    case ObjType::OBJ_TYPE_TANK:
        break;
    case ObjType::OBJ_TYPE_BULLET:
        break;
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
            //object.mutable_bullet()->set_dir(parent->);

            break;
        }
    default:
        break;
    }
    object.set_objid(g_iObjectNum++);
    g_objectMap.insert(make_pair(object.objid(), object));
    return object.objid();
}

void server_object_tick()
{
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_single_tick);
}
