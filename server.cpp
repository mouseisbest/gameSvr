#include <iostream>
#include "server_cs_processor.h"

using namespace std;

int main(int argc, char **argv)
{
    if (NULL == argv)
    {
        return -1;
    }
    StartServer();
    return 0;
}
