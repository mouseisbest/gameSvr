#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "client_cs_processor.h"

using std::cout;
using std::endl;

using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using gameSvr::CSMessageS;
using gameSvr::CSMessageC;

using gameSvr::CmdID;

void TankGameClient::StartConnection() 
{
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<CSMessageC, CSMessageS> > stream(
        stub_->ClientMsgProcessor(&context));

    std::thread writer([stream]() {
        while (1)
        {
            CSMessageC msg;
            msg.set_cmd(CmdID::CS_CMD_LOGIN);       
            stream->Write(msg);
            cout << "Sent message" << endl;

            sleep(3);
            CSMessageS response;
            stream->Read(&response);
            cout << "Recv Message: userid:" << response.mutable_loginresult()->userid() 
            << ",ret:" << response.ret() << endl;
        }});
        writer.join();
    Status status = stream->Finish();
    if (!status.ok()) {
        std::cout << "RouteChat rpc failed." << std::endl;
    }
}


int connect_to_server()
{
    TankGameClient client(
        grpc::CreateChannel("localhost:50051",
            grpc::InsecureChannelCredentials()
            ));
    client.StartConnection();

    return 0;
}
