// 04: C++ PUB -> Python SUB with a compact BINARY payload (no JSON).
//
// Multipart message:
//   frame 1: topic   "imu.raw"                         (for SUB filtering)
//   frame 2: header  uint32 seq, uint64 timestamp_us   (little endian, packed)
//   frame 3: samples float32[6]  ax ay az gx gy gz     (little endian)
//
// Python decodes frame 2 with struct.unpack("<IQ") and frame 3 with
// numpy.frombuffer(dtype="<f4") - zero parsing overhead, 36 bytes instead of
// ~150 bytes JSON. Both sides must agree on byte order and layout!
#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <zmq.hpp>

#pragma pack(push, 1)
struct Header {
    std::uint32_t seq;
    std::uint64_t timestamp_us;
};
#pragma pack(pop)
static_assert(sizeof(Header) == 12, "unexpected padding");

static volatile std::sig_atomic_t g_running = 1;
static void on_signal(int) { g_running = 0; }

int main(int argc, char** argv) {
    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);
    const int count = argc > 1 ? std::atoi(argv[1]) : 0;    // 0 = endless
    const char* env = std::getenv("ZMQ_ENDPOINT");
    const std::string endpoint = env ? env : "tcp://127.0.0.1:5592";

    zmq::context_t context{1};
    zmq::socket_t pub{context, zmq::socket_type::pub};
    pub.set(zmq::sockopt::sndhwm, 1000);
    pub.bind(endpoint);
    std::cout << "[C++ PUB] binary IMU stream on " << endpoint << " (100 Hz)" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // slow joiner

    const std::string topic = "imu.raw";
    for (std::uint32_t seq = 0; g_running && (count == 0 || seq < static_cast<std::uint32_t>(count)); ++seq) {
        const auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        Header header{seq, static_cast<std::uint64_t>(now)};
        const float t = seq * 0.01f;
        std::array<float, 6> samples{std::sin(t), std::cos(t), 9.81f, 0.1f * t, 0.0f, -0.1f * t};

        pub.send(zmq::buffer(topic), zmq::send_flags::sndmore);
        pub.send(zmq::buffer(&header, sizeof(header)), zmq::send_flags::sndmore);
        pub.send(zmq::buffer(samples), zmq::send_flags::none);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "[C++ PUB] done" << std::endl;
    return 0;
}
