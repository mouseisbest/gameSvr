#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <map>
#include <vector>
#include "def.pb.h"
using std::map;
using std::vector;
using gameSvr::Object;
using gameSvr::ObjType;
using gameSvr::Direction;

typedef map<uint64_t, Object> OBJECT_MAP_TYPE;
typedef OBJECT_MAP_TYPE::reference OBJECT_ITEM_TYPE;

void server_object_position_change(Object &obj, Direction dir);
uint64_t server_object_create(ObjType objType, uint64_t param1, uint64_t param2);
int server_object_remove(uint64_t objId);
Object *server_object_find(uint64_t objid);
vector<Object*> server_object_find_by_pos(int x, int y);

void server_object_tick();

#endif

