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

using std::cout;
using std::endl;

using gameSvr::ObjType;
using gameSvr::AttrType;
using gameSvr::Tank;
using gameSvr::def_enum;
using gameSvr::res_enum;
using gameSvr::CmdID;
using gameSvr::CSObject;

int g_iObjectNum;
OBJECT_MAP_TYPE g_objectMap;
std::mutex g_objectMutex;
extern GameServerImpl g_networkService;



OBJECT_LIST_TYPE server_object_find_by_object(Object &object)
{
    OBJECT_LIST_TYPE result;
    auto objPos = object.mutable_position()->mutable_pos();
    for (auto it = g_objectMap.begin(); it != g_objectMap.end(); )
    {
        Object *pObject = &it->second;
        if (pObject->mutable_position()->mutable_pos()->x() == objPos->x() &&
            pObject->mutable_position()->mutable_pos()->y() == objPos->y() &&
            pObject->objid() != object.objid())
        {
            result.push_back(pObject);
        }
        ++it;
    }
    return result;
}



OBJECT_LIST_TYPE server_object_find_by_pos(int x, int y)
{
    OBJECT_LIST_TYPE result;
    for (auto it = g_objectMap.begin(); it != g_objectMap.end(); )
    {
        Object *pObject = &it->second;
        if (pObject->mutable_position()->mutable_pos()->x() == x &&
            pObject->mutable_position()->mutable_pos()->y() == y)
        {
            result.push_back(pObject);
        }
    }
    /*cout << "found " << nearbyObjList.size() << " objects in " << 
        object->mutable_position()->mutable_pos()->x() << "," <<
        object->mutable_position()->mutable_pos()->y() << endl;*/
    return result;
}

static void server_object_set_attr_dirty(Object &obj, int64_t iAttrType)
{
    int64_t flag = obj.flag();
    flag |= (1 << iAttrType);
    obj.set_flag(flag);
}


static void server_object_clear_attr_dirty(Object &obj, int64_t iAttrType)
{
    int64_t flag = obj.flag();
    flag &= (~(1 << iAttrType));
    obj.set_flag(flag);
}

static bool server_object_get_attr_flag(Object &obj, int64_t iAttrType)
{
    return obj.flag() & (1 << iAttrType);
}


static void server_object_set_hp(Object &obj, int iHP)
{
    obj.mutable_tank()->mutable_battleinfo()->set_hp(iHP);
    cout << __FUNCTION__ << "Obj " << obj.objid() << " current hp is: " << obj.mutable_tank()->mutable_battleinfo()->hp() << endl;
    server_object_set_attr_dirty(obj, AttrType::ATTR_HP);
}

void server_object_position_change(Object &obj, Direction dir)
{
    auto pos = obj.mutable_position();

    int x = pos->mutable_pos()->x();
    int y = pos->mutable_pos()->y();
    int speed = 0; 
    if (obj.type() == ObjType::OBJ_TYPE_TANK)
    {
        auto info = obj.mutable_tank()->mutable_battleinfo();
        speed = info->speed();
    }
    else
    {
        speed = obj.mutable_bullet()->speed();
    }
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
        if (obj.type() != ObjType::OBJ_TYPE_TANK)
        {
            obj.set_collision(1);
        }
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
    //cout << "dir" << pos->dir() << " x:" << pos->mutable_pos()->x() << " y:" << pos->mutable_pos()->y() << endl;
}


void server_object_tick_bullet(Object &obj)
{

    static int iTickCount = 0;
    iTickCount = (++iTickCount) % 5;
    if (0 == iTickCount)
    {
        server_object_position_change(obj, obj.mutable_position()->dir());
    }
}


static void server_object_move_tick(OBJECT_ITEM_TYPE item)
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

static void server_object_print_all_tank_hp()
{
    for (auto it = g_objectMap.begin(); it != g_objectMap.end(); ++it)
    {
        Object *pObject = &it->second;
        if (pObject->type() != ObjType::OBJ_TYPE_TANK)
        {
            continue;
        }
        cout << "tank " << pObject->objid() << " hp is: " << pObject->mutable_tank()->
            mutable_battleinfo()->hp() << endl;
    }
}

static void server_object_combat_tick(OBJECT_ITEM_TYPE item)
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
            OBJECT_LIST_TYPE nearbyObjList = server_object_find_by_object(*object);
            Object *relatedObj = server_object_find_no_mutex(object->mutable_bullet()->linkobj());
            if (nullptr == relatedObj)
            {
                //cout << "bullet " << object->objid() << "'s tank obj not found." << endl;
                break;
            }
            if (nearbyObjList.size())
            {
                /*cout << "found " << nearbyObjList.size() << " objects in " << 
                    object->mutable_position()->mutable_pos()->x() << "," <<
                    object->mutable_position()->mutable_pos()->y() << endl;*/
            }
            int iIsHit = 0;
            for (int i = 0; i < nearbyObjList.size(); ++i)
            {
                Object *nearbyObj = nearbyObjList[i];
                if (nullptr == nearbyObj)
                {
                    continue;
                }
                // 如果是坦克，则给它减血
                if (ObjType::OBJ_TYPE_TANK == nearbyObj->type())
                {
                    if (nearbyObj->objid() == relatedObj->objid())
                    {
                        continue;
                    }
                    int iCurHp = nearbyObj->mutable_tank()->mutable_battleinfo()->hp();
                    iCurHp -= relatedObj->mutable_tank()->mutable_battleinfo()->damage();
                    printf("%s:tank(%ld) is hit by tank(%ld)'s bullet(%ld)\n",
                        __FUNCTION__, nearbyObj->objid(), relatedObj->objid(), 
                        object->objid());
                    server_object_set_hp(*nearbyObj, iCurHp);
                    server_object_print_all_tank_hp();
                    iIsHit = 1;
                }
            }
            if (iIsHit)
            {
                object->set_collision(1);
            }
             
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
    info->set_lastfiretime(get_time());
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
    if (get_time() - tank->mutable_battleinfo()->lastfiretime() < def_enum::MIN_TANK_FIRE_TIME)
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
            // cout << "try bullet " << now << endl;
            int iRet = server_object_create_bullet_check(parent, now);
            if (iRet)
            {
                return iRet;
            }
            auto tank = parent->mutable_tank();
            tank->mutable_battleinfo()->set_lastfiretime(now);
            //cout << tank->mutable_battleinfo()->lastfiretime() << endl;
            object.set_type(ObjType::OBJ_TYPE_BULLET);
            object.mutable_position()->mutable_pos()->set_x(parent->mutable_position()->mutable_pos()->x());
            object.mutable_position()->mutable_pos()->set_y(parent->mutable_position()->mutable_pos()->y());
            object.mutable_position()->set_dir(parent->mutable_position()->dir());
            object.mutable_bullet()->set_speed(def_enum::DEFAULT_BULLET_SPEED);
            object.mutable_bullet()->set_linkobj(parent->objid());
            // cout << "tank objid: " << parent->objid() << " fired" << endl;
            // cout << "bullet created " << object.objid() << endl;

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

Object *server_object_find_no_mutex(uint64_t objid)
{
    auto res = g_objectMap.find(objid);
    if (res == g_objectMap.end())
    {
        return nullptr;
    }
    return &res->second;
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
//            battle_info->set_hp(obj.mutable_tank()->mutable_battleinfo()->hp());
            battle_info->set_dir(obj.mutable_position()->dir());
            break;
        }
    case ObjType::OBJ_TYPE_BULLET:
        {
            csObj.mutable_bullet()->set_dir(obj.mutable_position()->dir());
            csObj.mutable_bullet()->set_parentid(obj.mutable_bullet()->linkobj());
            
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
    //printf("%s: %d objects packed\n", __FUNCTION__, array->object_size());
    g_networkService.BroadcastMsg(msg);
}

static void server_attr_broadcast()
{
    if (g_objectMap.size() == 0)
    {
        return;
    }
    for (OBJECT_MAP_TYPE::iterator it = g_objectMap.begin(); it != g_objectMap.end(); ++it)
    {
        CSMessageS msg;
        msg.set_cmd(CmdID::CS_CMD_ATTR_SYNC);
        auto array = msg.mutable_attrsync();
        Object *obj = &it->second;
        array->set_objid(obj->objid());
        if (obj->type() != ObjType::OBJ_TYPE_TANK)
        {
            continue;
        }
        for (int i = 0; i < AttrType::ATTR_MAX; ++i)
        {
            if (obj->flag())
                cout << "obj " << obj->objid() << " flag is:" << obj->flag() << endl;;
            if (server_object_get_attr_flag(*obj, i)) // 标记有数据更改
            {
                auto csAttr = array->add_syncdata();
                csAttr->set_attrtype((AttrType)i);
                switch (i)
                {
                case AttrType::ATTR_HP:
                    csAttr->set_value(obj->mutable_tank()->mutable_battleinfo()->hp());
                    server_object_clear_attr_dirty(*obj, AttrType::ATTR_HP);
                    break;
                default:
                    break;
                }
            }
        }
        if (array->syncdata_size() <= 0)
        {
            continue;
        }
        printf("%s: %d attr change packed\n", __FUNCTION__, array->syncdata_size());
        g_networkService.BroadcastMsg(msg);
    }
}


void server_object_tick()
{
    std::unique_lock<std::mutex> lock(g_objectMutex);
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_move_tick);
    for_each(g_objectMap.begin(), g_objectMap.end(), server_object_combat_tick);
    for (auto it = g_objectMap.begin(); it != g_objectMap.end(); )
    {
        if (it->second.collision())
        {
            g_objectMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    server_object_broadcast();
    server_attr_broadcast();
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
