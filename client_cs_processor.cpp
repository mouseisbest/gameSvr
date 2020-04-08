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
using std::shared_ptr;

using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using gameSvr::CSMessageS;
using gameSvr::CSMessageC;
using gameSvr::CSLoginC;

using gameSvr::CmdID;

void TankGameClient::StartConnection() 
{
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<CSMessageC, CSMessageS> > stream(
        stub_->ClientMsgProcessor(&context));

    while (1)
    {
        std::thread writer([stream]() {
            CSMessageC msg;
            msg.set_cmd(CmdID::CS_CMD_LOGIN);       
            CSLoginC *loginInfo = new CSLoginC();
            loginInfo->set_username("testuser");
            loginInfo->set_password("123");
            msg.set_allocated_logininfo(loginInfo);
            int iRet = stream->Write(msg);
            cout << "Sent message result:" << iRet  << endl;
        });

        writer.join();
        CSMessageS response;
        int iRet = stream->Read(&response);
        cout << "Recv Message:" << iRet <<  ",userid:" << response.mutable_loginresult()->token() 
            << ",ret:" << response.ret() << endl;
        sleep(1);
    }
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
