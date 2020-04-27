#ifndef __CLIENT_CS_PROCESSOR_H__
#define __CLIENT_CS_PROCESSOR_H__

#include <deque>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/channel.h>

#include "key.pb.h"
#include "cs.grpc.pb.h"

using gameSvr::GameServer;
using gameSvr::Direction;
using grpc::Channel;
using grpc::ClientContext;

using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using gameSvr::CSMessageS;
using gameSvr::CSMessageC;


typedef ClientReaderWriter<CSMessageC, CSMessageS> SERVERSTREAM;
class TankGameClient 
{
public:
    TankGameClient(std::shared_ptr<Channel> channel, std::string user_name, std::string password)
;
    ~TankGameClient();

    void WaitForThreads();

    int SendMoveReq(Direction dir);
    int SendFireReq();

private:
    int TryToLogin();
    static void SendMessageThread(void *parm);
    static void RecvMessageThread(void *parm);
    int ProcessServerMessage(CSMessageS &msg);
    void SendMessage(CSMessageC &msg);
    std::unique_ptr<GameServer::Stub>       stub_;
    ClientContext                           context_;
    std::shared_ptr<SERVERSTREAM>           stream_;
    std::thread                             readerThread_;
    std::thread                             writerThread_;
    std::deque<CSMessageC>                  queueSend_;
    std::deque<CSMessageS>                  queueRecv_;
    uint64_t                                token_;
    std::string                             userName;
    std::string                             password;
    
};


#endif
