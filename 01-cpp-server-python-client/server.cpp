#include <cstdlib>
#include <iostream>
#include <string>
#include <zmq.hpp>
int main(){
 const char* env=std::getenv("ZMQ_ENDPOINT"); const std::string endpoint=env?env:"tcp://127.0.0.1:5590";
 zmq::context_t ctx(1); zmq::socket_t s(ctx,zmq::socket_type::rep); s.bind(endpoint); std::cout<<"REP server on "<<endpoint<<" (Ctrl-C to stop)\n";
 while(true){zmq::message_t req; auto ok=s.recv(req,zmq::recv_flags::none); if(!ok)continue; std::string text(static_cast<char*>(req.data()),req.size()); std::cout<<"received: "<<text<<"\n"; std::string reply="{\"ok\":true,\"reply\":\"Hello from C++\"}"; s.send(zmq::buffer(reply),zmq::send_flags::none);}
}
