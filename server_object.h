#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <map>
#include "def.pb.h"
using std::map;
using gameSvr::Object;
using gameSvr::ObjType;

typedef map<uint64_t, Object> OBJECT_MAP_TYPE;
typedef OBJECT_MAP_TYPE::reference OBJECT_ITEM_TYPE;

uint64_t server_object_create(ObjType objType, uint64_t param1, uint64_t param2);

void server_object_tick();

#endif

