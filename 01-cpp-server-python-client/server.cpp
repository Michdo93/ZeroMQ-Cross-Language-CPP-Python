// 01: C++ REP server <-> Python REQ client
// Build: cmake -S . -B build && cmake --build build && ./build/server
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <zmq.hpp>

static volatile std::sig_atomic_t g_running = 1;
static void on_signal(int) { g_running = 0; }

int main() {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);

    const char* env = std::getenv("ZMQ_ENDPOINT");
    const std::string endpoint = env ? env : "tcp://127.0.0.1:5590";

    zmq::context_t context{1};                       // 1 I/O thread
    zmq::socket_t socket{context, zmq::socket_type::rep};
    socket.set(zmq::sockopt::linger, 0);
    socket.bind(endpoint);
    std::cout << "[C++ server] REP on " << endpoint << " (Ctrl+C to stop)" << std::endl;

    while (g_running) {
        // Poll with timeout so that Ctrl+C is handled promptly.
        zmq::pollitem_t items[] = {{socket.handle(), 0, ZMQ_POLLIN, 0}};
        try {
            zmq::poll(items, 1, std::chrono::milliseconds(500));
        } catch (const zmq::error_t& e) {
            if (e.num() == EINTR) continue;   // interrupted by Ctrl+C/SIGTERM -> re-check g_running
            throw;
        }
        if (!(items[0].revents & ZMQ_POLLIN)) continue;

        zmq::message_t request;
        if (!socket.recv(request, zmq::recv_flags::none)) continue;
        // ZeroMQ transports bytes - Python's send_json() sends UTF-8 JSON text.
        std::cout << "[C++ server] received: " << request.to_string() << std::endl;

        const std::string reply = R"({"ok": true, "reply": "Hello from C++"})";
        socket.send(zmq::buffer(reply), zmq::send_flags::none);
    }
    std::cout << "[C++ server] stopped" << std::endl;
    return 0;
}
