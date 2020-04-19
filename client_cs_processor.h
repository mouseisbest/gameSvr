#ifndef __CLIENT_CS_PROCESSOR_H__
#define __CLIENT_CS_PROCESSOR_H__

#include <deque>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/channel.h>

#include "cs.grpc.pb.h"

using grpc::Channel;
using gameSvr::GameServer;
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
    TankGameClient(std::shared_ptr<Channel> channel);
    ~TankGameClient();

    void SendMessage(CSMessageC &msg);

    static void SendMessageThread(void *parm);
    static void RecvMessageThread(void *parm);

    int ProcessServerMessage(CSMessageS &msg);

    void WaitForThreads();

private:
    std::unique_ptr<GameServer::Stub>       stub_;
    ClientContext                           context_;
    std::shared_ptr<SERVERSTREAM>           stream_;
    std::mutex                              mu_;
    std::thread                             readerThread_;
    std::thread                             writerThread_;
    std::deque<CSMessageC>                  queueSend_;
    std::deque<CSMessageS>                  queueRecv_;
    uint64_t                                token_;
    
};







#endif
