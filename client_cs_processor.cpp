#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>
#include "client_cs_processor.h"

using std::cout;
using std::endl;
using std::shared_ptr;

using grpc::Status;

using gameSvr::CSLoginC;

using gameSvr::CmdID;


TankGameClient::TankGameClient(std::shared_ptr<Channel> channel)
        : stub_(GameServer::NewStub(channel)) {
        //stream_ = new ClientReaderWriter<CSMessageC, CSMessageS> (stub_->ClientMsgProcessor(&context));
        }

TankGameClient::~TankGameClient()
{
}



void TankGameClient::StartConnection() 
{
    ClientContext context;

    std::shared_ptr<ClientReaderWriter<CSMessageC, CSMessageS> > stream(
        stub_->ClientMsgProcessor(&context));

    //while (1)
    //{
        std::thread writer([stream]() {
            CSMessageC msg;
            msg.set_cmd(CmdID::CS_CMD_LOGIN);       
            msg.set_seq(time(NULL));
            msg.mutable_logininfo()->set_username("test_user");
            msg.mutable_logininfo()->set_password("123");
            int iRet = stream->Write(msg);
            cout << "Sent message result:" << iRet  << endl;
        });

        writer.join();
        CSMessageS response;
        int iRet = stream->Read(&response);
        cout << "Recv Message:" << iRet <<  ",userid:" << response.mutable_loginresult()->token() 
            << ",ret:" << response.ret() << endl;
        sleep(1);
    //}
    //Status status = stream->Finish();
    //if (!status.ok()) {
    //    std::cout << "RouteChat rpc failed." << std::endl;
    //}
}


int connect_to_server()
{
    TankGameClient client(
        grpc::CreateChannel("localhost:50051",
            grpc::InsecureChannelCredentials()
            ));
    for (;;)
        client.StartConnection();

    return 0;
}
