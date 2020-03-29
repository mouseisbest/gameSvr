
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "cs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using gameSvr::GameServer;
using gameSvr::CSMessageS;
using gameSvr::CSMessageC;

class TankGameClient 
{
public:
    TankGameClient(std::shared_ptr<Channel> channel)
        : stub_(GameServer::NewStub(channel)) {
        }
/*    void RouteChat() 
    {
        ClientContext context;

        std::shared_ptr<ClientReaderWriter<RouteNote, RouteNote> > stream(
            stub_->RouteChat(&context));

        std::thread writer([stream]() {
            std::vector<RouteNote> notes{
            MakeRouteNote("First message", 0, 0),
            MakeRouteNote("Second message", 0, 1),
            MakeRouteNote("Third message", 1, 0),
            MakeRouteNote("Fourth message", 0, 0)};
            for (const RouteNote& note : notes) {
            std::cout << "Sending message " << note.message()
            << " at " << note.location().latitude() << ", "
            << note.location().longitude() << std::endl;
            stream->Write(note);
            }
            stream->WritesDone();
            });

        RouteNote server_note;
        while (stream->Read(&server_note)) {
            std::cout << "Got message " << server_note.message()
                << " at " << server_note.location().latitude() << ", "
                << server_note.location().longitude() << std::endl;
        }
        writer.join();
        Status status = stream->Finish();
        if (!status.ok()) {
            std::cout << "RouteChat rpc failed." << std::endl;
        }
    }
*/
private:
    std::unique_ptr<GameServer::Stub> stub_;
};

int main(int argc, char** argv) {
    TankGameClient client(
        grpc::CreateChannel("localhost:50051",
            grpc::InsecureChannelCredentials()
        ));


    return 0;
}

