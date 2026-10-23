#include <cstdlib>
#include <iostream>
#include <string>
#include <zmq.hpp>
int main(){
 const char* env=std::getenv("ZMQ_ENDPOINT");const std::string endpoint=env?env:"tcp://127.0.0.1:5590";
 zmq::context_t ctx(1);zmq::socket_t s(ctx,zmq::socket_type::req);s.set(zmq::sockopt::rcvtimeo,3000);s.connect(endpoint);
 std::string request="{\"message\":\"hello from C++\"}";s.send(zmq::buffer(request),zmq::send_flags::none);zmq::message_t reply;
 if(!s.recv(reply,zmq::recv_flags::none)){std::cerr<<"Timed out waiting for Python server\n";return 1;}
 std::cout<<std::string(static_cast<char*>(reply.data()),reply.size())<<"\n";
}
