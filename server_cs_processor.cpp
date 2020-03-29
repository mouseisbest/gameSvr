#include "server_cs_processor.h"



Status GameServerImpl::ClientMsgProcessor(ServerContext* context,
        ServerReaderWriter<CSMessageS, CSMessageC>* stream) {

        CSMessageC msg;
        while (stream->Read(&msg)) {
            ;
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
