#ifndef __CLIENT_CS_PROCESSOR_H__
#define __CLIENT_CS_PROCESSOR_H__




#include <grpcpp/channel.h>

#include "cs.grpc.pb.h"

using grpc::Channel;
using gameSvr::GameServer;

class TankGameClient 
{
public:
    TankGameClient(std::shared_ptr<Channel> channel)
        : stub_(GameServer::NewStub(channel)) {
        }

    void StartConnection();

private:
    std::unique_ptr<GameServer::Stub> stub_;
};




int connect_to_server();



#endif
