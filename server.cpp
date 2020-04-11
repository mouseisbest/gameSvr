#include <iostream>
#include "server_tick.h"
#include "server_cs_processor.h"


int main(int argc, char **argv)
{
    if (NULL == argv)
    {
        return -1;
    }
    start_server();
    server_tick();
    return 0;
}
