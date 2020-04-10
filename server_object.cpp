#include <algorithm>

#include "server_object.h"

using std::for_each;
using gameSvr::ObjType;

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


void server_object_tick()
{
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_single_tick);
}
