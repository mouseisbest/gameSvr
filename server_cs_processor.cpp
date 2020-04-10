
#include <thread>
#include <iostream>

#include "cs.pb.h"
#include "server_cs_processor.h"

using gameSvr::CmdID;
using std::cout;
using std::endl;



extern PLAYER_MAP_TYPE g_userMap;

GameServerImpl g_networkService;
auto g_networkFun = []() {
    std::string server_address("0.0.0.0:50051");
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&g_networkService);
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
                cout << "incoming username:" << loginInfo->username() << ", password" << 
                    loginInfo->password() << endl;
                if (NULL != player)
                {
                    ret.set_cmd(msg.cmd());
                    ret.mutable_loginresult()->set_token(player->token());
                    player->set_context((int64_t)stream);
                }
                ret.set_ret(time(NULL));
                int iRet = stream->Write(ret);
                cout << "write to client ret:" << iRet << endl;
                break;
            }
        case CmdID::CS_CMD_MOVE:
            {
                break;
            }
        case CmdID::CS_CMD_FIRE:
            {
                break;
            }
        default:
            break;
        }
    }
    cout << "Client connection closed." << endl;
    return Status::OK;
}


int GameServerImpl::SendMessage(Player *player, CSMessageS &msg)
{
    if (NULL == player || 0 == player->mutex() || 0 == player->context())
    {
        return -1;
    }

    std::mutex *mutex = (std::mutex*)player->mutex();
    std::unique_lock<std::mutex> lock(*mutex);
    LPCLIENTCONTEXT stream = (LPCLIENTCONTEXT)player->context();
    stream->Write(msg);
    return 0;
}


int GameServerImpl::BoradcastMsg(CSMessageS &msg)
{
    for (PLAYER_MAP_TYPE::iterator it = g_userMap.begin(); 
        it != g_userMap.end(); ++it)
    {
        SendMessage(&it->second, msg);
    }
    return 0;
}

int start_server()
{
    g_networkThread = std::thread(g_networkFun);
    return 0;
}
