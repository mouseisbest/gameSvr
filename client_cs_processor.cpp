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
    : stub_(GameServer::NewStub(channel)), 
    context_(), 
    stream_(stub_->ClientMsgProcessor(&context_)),
    readerThread_(&TankGameClient::RecvMessageThread, this),
    writerThread_(&TankGameClient::SendMessageThread, this)
{
}

TankGameClient::~TankGameClient()
{
}



void TankGameClient::SendMessage(CSMessageC &msg) 
{
    std::unique_lock<std::mutex> lock(mu_);
    msg.set_token(token_);
    msg.set_cmd(CmdID::CS_CMD_LOGIN);       
    msg.set_seq(time(NULL));
    msg.mutable_logininfo()->set_username("test_user");
    msg.mutable_logininfo()->set_password("123");
    queueSend_.push_back(msg);
    cout << "Sent message to queue" << endl;
}


void TankGameClient::SendMessageThread(void *parm)
{
    if (nullptr == parm)
    {
        return;
    }
    TankGameClient *client = (TankGameClient*)parm;
    while (1)
    {
        std::unique_lock<std::mutex> lock(client->mu_);
        if (client->queueSend_.empty())
        {
            usleep(1);
            continue;
        }
        int iRet = client->stream_->Write(client->queueSend_.front());
        client->queueSend_.pop_front();
        cout << "Send msg in queue ret:" << iRet << endl;
    }
}


void TankGameClient::RecvMessageThread(void *parm)
{
    if (nullptr == parm)
    {
        return;
    }
    TankGameClient *client = (TankGameClient*)parm;
    while (1)
    {
        CSMessageS response;
        int iRet = client->stream_->Read(&response);
        if (iRet != true)
        {
           continue;
        }
        iRet = client->ProcessServerMessage(response);
        if (iRet)
        {
            continue;
        }
        cout << "Recv Message:" << iRet <<  ",userid:" << response.mutable_loginresult()->token() 
            << ",ret:" << response.ret() << endl;
    }
}



int TankGameClient::ProcessServerMessage(CSMessageS &msg)
{
    switch (msg.cmd())
    {
    case CmdID::CS_CMD_LOGIN:
        {
            if (!msg.has_loginresult())
            {
                return -1;
            }
            token_ = msg.mutable_loginresult()->token();
        }
        break;
    }

    return 0;
}


void TankGameClient::WaitForThreads()
{
    readerThread_.join();
    writerThread_.join();
}


