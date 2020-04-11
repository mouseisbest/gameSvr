#include "time_tool.h"

#define PROTECT_MS 1000000

unsigned int time_diff_ms(struct timeval *time1, struct timeval *time2)
{
    int second = time1->tv_sec - time2->tv_sec;
    if (second < -PROTECT_MS)
    {
        second =  -PROTECT_MS;
    }
    else if (second > PROTECT_MS)
    {
        second = PROTECT_MS; 
    }
    return second * 1000 + (time1->tv_usec - time2->tv_usec)/1000;
}


