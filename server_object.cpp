#include <algorithm>

#include "server_object.h"

using std::for_each;

OBJECT_MAP_TYPE g_objectMap;

void server_object_single_tick(OBJECT_ITEM_TYPE item)
{

}


void server_object_tick()
{
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_single_tick);
}
