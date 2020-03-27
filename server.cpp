#include <iostream>

using namespace std;

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "cs.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using gameSvr::GameServer;
using gameSvr::Equation;
using gameSvr::EquationResult;
using std::chrono::system_clock;

int GetEquationResult(const Equation& equation)
{
    if (equation.op() == 1)
    {
        return equation.num1() + equation.num2();
    }
    return 0;
}


class GameServerImpl final : public GameServer::Service
{
public:
    explicit GameServerImpl(){};

    Status MathOp(ServerContext* context, const Equation* equation,
                  EquationResult* result) override {
        result->mutable_equation()->CopyFrom(*equation);
        result->set_result(GetEquationResult(*equation));
        return Status::OK;
    }


};



int main(int argc, char **argv)
{
    if (NULL == argv)
    {
        return -1;
    }

    std::string server_address("0.0.0.0:50051");
    GameServerImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();

    return 0;
}
