
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "time_tool.h"
#include "server_tick.h"

#define TICK_INTERVAL_US 1000

static int server_tick_100ms(timeval *now)
{
    if (now && now->tv_usec % 10000 != 0)
    {
        return 0;
    }

    return 0;
}


static int server_tick_1s(timeval *now)
{
    if (!now)
    {
        return 0;
    }
    static timeval last_tv = {};
    if (0 == last_tv.tv_sec)
    {
        memcpy(&last_tv, now, sizeof(last_tv));
    }

    if (time_diff_ms(now, &last_tv) < 1000)
    {
        return 0;
    }

    memcpy(&last_tv, now, sizeof(last_tv));
    printf("1\n");

    return 0;
}


static int server_tick_1min(timeval *now)
{
    if (now && now->tv_usec % 60000000 != 0)
    {
        return 0;
    }

    return 0;
}



int server_tick()
{
    timeval tv;
    while (1)
    {
        gettimeofday(&tv, NULL);
        server_tick_100ms(&tv);
        server_tick_1s(&tv);
        server_tick_1min(&tv);
        usleep(TICK_INTERVAL_US);
    }
    return 0;
}
