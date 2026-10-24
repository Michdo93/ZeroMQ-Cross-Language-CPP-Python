// 02: C++ REQ client <-> Python REP server
#include <cstdlib>
#include <iostream>
#include <string>
#include <zmq.hpp>

int main() {
    const char* env = std::getenv("ZMQ_ENDPOINT");
    const std::string endpoint = env ? env : "tcp://127.0.0.1:5590";

    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.set(zmq::sockopt::rcvtimeo, 3000);   // never block forever
    socket.set(zmq::sockopt::linger, 0);
    socket.connect(endpoint);

    const std::string request = R"({"message": "hello from C++"})";
    socket.send(zmq::buffer(request), zmq::send_flags::none);

    zmq::message_t reply;
    if (!socket.recv(reply, zmq::recv_flags::none)) {   // empty optional = timeout
        std::cerr << "[C++ client] timeout - is the Python server running?" << std::endl;
        return 1;
    }
    std::cout << "[C++ client] reply: " << reply.to_string() << std::endl;
    return 0;
}
