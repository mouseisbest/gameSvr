#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <map>
#include "def.pb.h"
using std::map;
using gameSvr::Object;

typedef map<uint64_t, Object> OBJECT_MAP_TYPE;
typedef OBJECT_MAP_TYPE::reference OBJECT_ITEM_TYPE;


void server_object_tick();

#endif

