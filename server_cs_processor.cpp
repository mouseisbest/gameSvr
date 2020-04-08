
#include <thread>
#include <iostream>

#include "cs.pb.h"
#include "server_cs_processor.h"
#include "server_user.h"

using gameSvr::CmdID;
using std::cout;
using std::endl;


auto g_networkFun = []() {
    std::string server_address("0.0.0.0:50051");
    GameServerImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
};
std::thread g_networkThread;



Status GameServerImpl::ClientMsgProcessor(ServerContext* context,
    ServerReaderWriter<CSMessageS, CSMessageC>* stream) 
{
    int iRet = -1;
    CSMessageC msg;
    while (stream->Read(&msg)) 
    {
        std::unique_lock<std::mutex> lock(mu_); 
        printf("incoming msg (%d)\n", msg.cmd());
        CSMessageS ret;
        switch (msg.cmd())
        {
        case CmdID::CS_CMD_LOGIN:
            {
                auto loginInfo = msg.mutable_logininfo();
                iRet = server_user_login(loginInfo->username(), 
                    loginInfo->password());
                Player* player = server_user_get_by_name(loginInfo->username());
                // debug
                cout << "incoming username:" << loginInfo->username() << ", password" << 
                    loginInfo->password() << endl;
                if (NULL != player)
                {
                    ret.set_cmd(msg.cmd());
                    ret.mutable_loginresult()->set_token(12345);
                    player->set_context((int64_t)stream);
                }
                ret.set_ret(666);
                int iRet = stream->Write(ret);
                cout << "write to client ret:" << iRet << endl;
                break;
            }
        case CmdID::CS_CMD_MOVE:
            {
                break;
            }
        default:
            break;
        }
    }
    cout << "Server network loop stopped." << endl;
    return Status::OK;
}


void GameServerImpl::GetMutex()
{
}
void GameServerImpl::ReleaseMutex()
{
}



int start_server()
{
    g_networkThread = std::thread(g_networkFun);
    return 0;
}
