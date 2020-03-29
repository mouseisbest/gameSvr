#include "cs.pb.h"
#include "server_cs_processor.h"
#include "server_user.h"

using gameSvr::CmdID;

Status GameServerImpl::ClientMsgProcessor(ServerContext* context,
    ServerReaderWriter<CSMessageS, CSMessageC>* stream) 
{
    int iRet = -1;
    CSMessageC msg;
    while (stream->Read(&msg)) {
        switch (msg.cmd())
        {
        case CmdID::CS_CMD_LOGIN:
            {
                auto loginInfo = msg.mutable_logininfo();
                iRet = server_user_login(loginInfo->username(), 
                    loginInfo->password());
                break;
            }
        default:
            break;
        }
    }
    return Status::OK;
}

int StartServer()
{
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
