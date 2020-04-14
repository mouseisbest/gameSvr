#include <algorithm>

#include "server_object.h"
#include "res.pb.h"

using std::for_each;
using gameSvr::ObjType;
using gameSvr::res_enum;

int g_iObjectCount;
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

uint64_t server_object_create(ObjType objType, uint64_t param1, uint64_t param2)
{
    uint64_t objID = 0;
    Object object;
    switch (objType)
    {
    case ObjType::OBJ_TYPE_TANK:
        object.set_type(ObjType::OBJ_TYPE_TANK);
        object.mutable_tank()->set_userid(param1);
        server_object_init_tank(object);
        break;
    case ObjType::OBJ_TYPE_BULLET:
        break;
    default:
        break;
    }
    return objID;
}

void server_object_tick()
{
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_single_tick);
}
