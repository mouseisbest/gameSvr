
#define DARWIN_FORCE_BUILTIN

#include <iostream>
#include <unistd.h>
#include "lib/graphics/darwin.hpp"
#include "key.pb.h"
#include "client_cs_processor.h"


using std::cout;
using gameSvr::Direction;

#include <string>
#include <random>
#include <array>
#include <deque>
#include <ctime>
using position_type=std::array<int,2>;
wchar_t wchUp = U'↑';
darwin::pixel head_pix(wchUp ,true,false,darwin::colors::white,darwin::colors::black);


position_type tank= {0, 0};
int heading=2;
/*
1=left
2=right
-1=up
-2=down
*/
bool god_mode=true;
void start(TankGameClient *client )
{
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(30);
	tank= {0, (int)(0.5*pic->get_height())};
	int frame=0;
    Direction dir = Direction::DIRECTION_EAST;
	while(true) {
		clock.reset();
		if(darwin::runtime.is_kb_hit()) {
			switch(darwin::runtime.get_kb_hit()) {
			case 'w':
				if(god_mode||heading!=-2)
                {
					heading=-1;
				    --tank[1];
                    head_pix.set_char('U');
                    dir = Direction::DIRECTION_NORTH;
                }
				break;
			case 's':
				if(god_mode||heading!=-1)
                {
					heading=-2;
				    ++tank[1];
                    head_pix.set_char('D');
                    dir = Direction::DIRECTION_SOUTH;
                }
				break;
			case 'a':
				if(god_mode||heading!=2)
                {
					heading=1;
				    --tank[0];
                    head_pix.set_char('L');
                    dir = Direction::DIRECTION_WEST;
                }
				break;
			case 'd':
				if(god_mode||heading!=1)
                {
					heading=2;
				    ++tank[0];
                    head_pix.set_char('R');
                    dir = Direction::DIRECTION_EAST;
                }
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
        //client->SendMoveReq(dir);
		pic->clear();
		pic->draw_pixel(tank[0],tank[1],head_pix);
		darwin::runtime.update_drawable();
		++frame;
		clock.sync();
	}
}


int main(int argc, char** argv) {

    TankGameClient client(
        grpc::CreateChannel("localhost:50051",
            grpc::InsecureChannelCredentials()));


	darwin::timer::delay(1000);
	darwin::runtime.load("./darwin.module");
	darwin::runtime.fit_drawable();

	auto pic = darwin::runtime.get_drawable();
    cout << pic->get_height();
	while(true) {
		heading=2;
		start(&client);
	}

    /*for (;;)
    {
        CSMessageC msg;
        client.SendMessage(msg);
        sleep(1);
    }*/




    client.WaitForThreads();

    return 0;
}

