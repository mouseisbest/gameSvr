#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <map>
#include "def.pb.h"
using std::map;
using gameSvr::Object;
using gameSvr::ObjType;
using gameSvr::Direction;

typedef map<uint64_t, Object> OBJECT_MAP_TYPE;
typedef OBJECT_MAP_TYPE::reference OBJECT_ITEM_TYPE;

void server_object_position_change(Object &obj, Direction dir);
uint64_t server_object_create(ObjType objType, uint64_t param1, uint64_t param2);
Object *server_object_find(uint64_t objid);

void server_object_tick();

#endif

