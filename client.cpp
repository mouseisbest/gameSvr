
#define DARWIN_FORCE_BUILTIN

#include <iostream>
#include <unistd.h>
#include "lib/graphics/darwin.hpp"
#include "client_cs_processor.h"


using std::cout;

#include <string>
#include <random>
#include <array>
#include <deque>
#include <ctime>
using position_type=std::array<int,2>;
wchar_t wchUp = U'↑';
darwin::pixel head_pix(wchUp ,true,false,darwin::colors::white,darwin::colors::black);

darwin::pixel tank_pix(wchUp,true,false,darwin::colors::white,darwin::colors::black);

position_type snake_head= {0,0};
int heading=2;
/*
1=left
2=right
-1=up
-2=down
*/
bool god_mode=true;
bool cross_wall=true;
int hard=3;
int score=0;

void die()
{
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(100);
	std::string str0="You die!Your score is "+std::to_string(score);
	std::string str1="Press any key to continue...";
	while(true) {
		clock.reset();
		if(darwin::runtime.is_kb_hit()) {
			darwin::runtime.get_kb_hit();
			break;
		}
		pic->clear();
		pic->fill(darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::red));
		pic->draw_string(0.5*(pic->get_width()-str0.size()),0.5*pic->get_height(),str0,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::red));
		pic->draw_string(0.5*(pic->get_width()-str1.size()),pic->get_height()-1,str1,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::red));
		darwin::runtime.update_drawable();
		clock.sync();
	}
}

void start()
{
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(30);
	snake_head= {0, (int)(0.5*pic->get_height())};
	int frame=0;
	while(true) {
		clock.reset();
		if(darwin::runtime.is_kb_hit()) {
			switch(darwin::runtime.get_kb_hit()) {
			case 'w':
				if(god_mode||heading!=-2)
                {
					heading=-1;
				    --snake_head[1];
                }
				break;
			case 's':
				if(god_mode||heading!=-1)
                {
					heading=-2;
				    ++snake_head[1];
                }
				break;
			case 'a':
				if(god_mode||heading!=2)
                {
					heading=1;
				    --snake_head[0];
                }
				break;
			case 'd':
				if(god_mode||heading!=1)
                {
					heading=2;
				    ++snake_head[0];
                }
				break;
			}
		}
        
		pic->clear();
		pic->draw_pixel(snake_head[0],snake_head[1],head_pix);
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
		start();
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

