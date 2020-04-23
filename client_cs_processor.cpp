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
    writerThread_(&TankGameClient::SendMessageThread, this),
    token_(0)
{
    TryToLogin();
}

TankGameClient::~TankGameClient()
{
}

int TankGameClient::TryToLogin()
{
    CSMessageC msg = {};
    msg.set_cmd(CmdID::CS_CMD_LOGIN);       
    msg.set_seq(time(NULL));
    msg.mutable_logininfo()->set_username("test_user");
    msg.mutable_logininfo()->set_password("123");
    SendMessage(msg);
    return 0;
}

void TankGameClient::SendMessage(CSMessageC &msg) 
{
    if (token_ > 0)
    {
        msg.set_token(token_);
    }
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
        if (client->queueSend_.empty())
        {
            usleep(100000);
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
           usleep(100000);
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
            //cout << "Got server token:" << token_ << endl;
        }
        break;
    case CmdID::CS_CMD_MOVE:
        {
        }
        break;
    default:
        break;
    }

    return 0;
}


void TankGameClient::WaitForThreads()
{
    readerThread_.join();
    writerThread_.join();
}


int TankGameClient::SendMoveReq(Direction dir)
{
    CSMessageC msg = {};
    msg.set_cmd(CmdID::CS_CMD_MOVE);
    msg.mutable_move()->set_dir(dir);
    SendMessage(msg);
    return 0;
}


int TankGameClient::SendFireReq()
{
    return 0;
}
