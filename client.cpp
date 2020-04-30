
#define DARWIN_FORCE_BUILTIN

#include <iostream>
#include <unistd.h>
#include "lib/graphics/darwin.hpp"
#include "key.pb.h"
#include "def.pb.h"
#include "client_cs_processor.h"


using std::cout;
using std::string;
using std::endl;
using gameSvr::Direction;
using gameSvr::CSMapInfoS;
using gameSvr::ObjType;


#include <string>
#include <random>
#include <array>
#include <deque>
#include <ctime>
using position_type=std::array<int,2>;
wchar_t wchUp = '#';
darwin::pixel head_pix(wchUp ,true,false,darwin::colors::white,darwin::colors::black);

CSMapInfoS g_objectList;
std::mutex g_objListMutex;
position_type tank= {0, 0};
int heading=2;
/*
1=left
2=right
-1=up
-2=down
*/
bool god_mode=true;

void draw_all_objects()
{
    auto pic = darwin::runtime.get_drawable();
    darwin::sync_clock clock(30);
    darwin::pixel obj_pixel('#', true, false, darwin::colors::white, darwin::colors::black);
    while (1)
    {
        usleep(100000);
        CSMapInfoS data;
        int iSize = 0;
        {
            std::unique_lock<std::mutex> lock(g_objListMutex);
            data = g_objectList;
            iSize = g_objectList.object_size();
        }
        pic->clear(); 
        for (int i = 0; i < iSize; ++i)
        {
            auto obj = g_objectList.object(i);
            auto pos = obj.mutable_position();
            switch (obj.type())
            {
            case ObjType::OBJ_TYPE_TANK:
                {
                    switch (pos->dir())
                    {
                    case Direction::DIRECTION_NORTH:
                        obj_pixel.set_char('U');
                        break;
                    case Direction::DIRECTION_SOUTH:
                        obj_pixel.set_char('D');
                        break;
                    case Direction::DIRECTION_WEST:
                        obj_pixel.set_char('L');
                        break;
                    case Direction::DIRECTION_EAST:
                        obj_pixel.set_char('R');
                        break;
                    default:
                        break;
                    }
                    break;
                }
            case ObjType::OBJ_TYPE_BULLET:
                {
                    obj_pixel.set_char('#');
                    break;
                }
            default:
                break;
            } 
            pic->draw_pixel(pos->mutable_pos()->x(), pos->mutable_pos()->y(), obj_pixel); 
            //printf("obj(%d,%d)\n", pos->mutable_pos()->x(), pos->mutable_pos()->y());
        } 
        darwin::runtime.update_drawable();

        clock.sync();
    }
}

void start(TankGameClient *client )
{
	darwin::sync_clock clock(30);
    Direction dir = Direction::DIRECTION_EAST;
	while(true) {
		if(darwin::runtime.is_kb_hit()) {
			switch(darwin::runtime.get_kb_hit()) {
			case 'w':
				if(god_mode||heading!=-2)
                {
					heading=-1;
				    --tank[1];
                    head_pix.set_char('U');
                    dir = Direction::DIRECTION_NORTH;
                    client->SendMoveReq(dir);
                }
                break;
            case 's':
                if(god_mode||heading!=-1)
                {
                    heading=-2;
                    ++tank[1];
                    head_pix.set_char('D');
                    dir = Direction::DIRECTION_SOUTH;
                    client->SendMoveReq(dir);
                }
                break;
            case 'a':
                if(god_mode||heading!=2)
                {
                    heading=1;
                    --tank[0];
                    head_pix.set_char('L');
                    dir = Direction::DIRECTION_WEST;
                    client->SendMoveReq(dir);
                }
                break;
            case 'd':
                if(god_mode||heading!=1)
                {
                    heading=2;
                    ++tank[0];
                    head_pix.set_char('R');
                    dir = Direction::DIRECTION_EAST;
                    client->SendMoveReq(dir);
                }
				break;
            case ' ':
                client->SendFireReq();
                break;
            default:
                break;
			}
		}
        if (tank[0] < 0)
        {
            tank[0] = 0;
        }
        if (tank[1] < 0)
        {
            tank[1] = 0;
        }
        if (tank[0] >= 60)
        {
            tank[0] = 60;
        }
        if (tank[1] >= 18)
        {
            tank[1] = 18;
        }
		//pic->clear();
		//pic->draw_pixel(tank[0],tank[1],head_pix);
		//darwin::runtime.update_drawable();
		//++frame;
		//clock.sync();
	}
}


int main(int argc, char** argv) {

    if (argc < 4)
    {
        cout << "usage: " << endl;
        cout << "\t client address username password" << endl;
        exit(1);
    }
    string address = string(argv[1]) + ":50051";
    string username = argv[2];
    string password = argv[3];
    TankGameClient client(
        grpc::CreateChannel(address,
        grpc::InsecureChannelCredentials()),
        username,
        password);


	darwin::timer::delay(1000);
	darwin::runtime.load("./darwin.module");
	darwin::runtime.fit_drawable();

    std::thread drawThread(draw_all_objects);
    start(&client);

    drawThread.join();
    client.WaitForThreads();

    return 0;
}

