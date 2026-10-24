// 03: Structured JSON between C++ (nlohmann/json) and Python (json / send_json)
//
// The C++ server receives a command object, validates it, computes something
// and answers with a JSON object. Invalid input is answered with an error
// object instead of crashing - a REP socket must always reply.
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <zmq.hpp>

using json = nlohmann::json;

static volatile std::sig_atomic_t g_running = 1;
static void on_signal(int) { g_running = 0; }

json handle(const json& req) {
    const std::string cmd = req.value("command", "");
    if (cmd == "stats") {
        const auto values = req.at("values").get<std::vector<double>>();
        if (values.empty()) return {{"ok", false}, {"error", "values must not be empty"}};
        const double sum = std::accumulate(values.begin(), values.end(), 0.0);
        const double mean = sum / values.size();
        double var = 0.0;
        for (double v : values) var += (v - mean) * (v - mean);
        return {{"ok", true},
                {"count", values.size()},
                {"mean", mean},
                {"stddev", std::sqrt(var / values.size())},
                {"min", *std::min_element(values.begin(), values.end())},
                {"max", *std::max_element(values.begin(), values.end())},
                {"server", "C++ / nlohmann::json"}};
    }
    return {{"ok", false}, {"error", "unknown command: " + cmd}};
}

int main() {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);
    const char* env = std::getenv("ZMQ_ENDPOINT");
    const std::string endpoint = env ? env : "tcp://127.0.0.1:5591";

    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::rep};
    socket.set(zmq::sockopt::linger, 0);
    socket.bind(endpoint);
    std::cout << "[C++ JSON server] on " << endpoint << std::endl;

    while (g_running) {
        zmq::pollitem_t items[] = {{socket.handle(), 0, ZMQ_POLLIN, 0}};
        try {
            zmq::poll(items, 1, std::chrono::milliseconds(500));
        } catch (const zmq::error_t& e) {
            if (e.num() == EINTR) continue;   // interrupted by Ctrl+C/SIGTERM -> re-check g_running
            throw;
        }
        if (!(items[0].revents & ZMQ_POLLIN)) continue;

        zmq::message_t msg;
        if (!socket.recv(msg, zmq::recv_flags::none)) continue;

        json reply;
        try {
            const json req = json::parse(msg.to_string_view());
            std::cout << "[C++ JSON server] request: " << req.dump() << std::endl;
            reply = handle(req);
        } catch (const json::exception& e) {   // parse error, missing key, wrong type
            reply = {{"ok", false}, {"error", e.what()}};
        }
        const std::string out = reply.dump();
        socket.send(zmq::buffer(out), zmq::send_flags::none);
    }
    return 0;
}
