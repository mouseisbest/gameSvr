#ifndef __SERVER_CS_PROCESSOR_H__
#define __SERVER_CS_PROCESSOR_H__


#include <chrono>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "cs.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using gameSvr::GameServer;
using gameSvr::CSMessageS;
using gameSvr::CSMessageC;
using std::chrono::system_clock;
class GameServerImpl final : public GameServer::Service
{
public:
    explicit GameServerImpl(){};
    Status ClientMsgProcessor(ServerContext* context,
        ServerReaderWriter<CSMessageS, CSMessageC>* stream) override;


    private:
        std::mutex mu_;
};

int StartServer();

#endif
