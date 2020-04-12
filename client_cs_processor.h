#ifndef __CLIENT_CS_PROCESSOR_H__
#define __CLIENT_CS_PROCESSOR_H__



#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/channel.h>

#include "cs.grpc.pb.h"

using grpc::Channel;
using gameSvr::GameServer;
using grpc::ClientContext;

using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using gameSvr::CSMessageS;
using gameSvr::CSMessageC;

typedef ClientReaderWriter<CSMessageC, CSMessageS> SERVERSTREAM;
typedef SERVERSTREAM * LPSERVERSTREAM;
class TankGameClient 
{
public:
    TankGameClient(std::shared_ptr<Channel> channel);
    ~TankGameClient();

    void SendMessage(CSMessageC &msg);

private:
    std::unique_ptr<GameServer::Stub> stub_;
    ClientReaderWriter<CSMessageC, CSMessageS> *stream_;
    ClientContext context_;
};




int connect_to_server();



#endif
