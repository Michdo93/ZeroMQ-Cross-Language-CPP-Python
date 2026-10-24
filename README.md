# ZeroMQ Cross-Language: C++ ⇄ Python

ZeroMQ only transports **bytes in frames**. As long as both sides agree on the socket pattern, the endpoint and the message format, a C++ program and a Python program can talk to each other directly – no wrappers, no code generator.

Companion repository of the book *From Zero to Hero – ZeroMQ in Python meistern*.

| # | Folder | C++ | Python | Message format |
|--:|--------|-----|--------|----------------|
| 1 | [`01-cpp-server-python-client`](01-cpp-server-python-client) | REP server | REQ client | UTF-8 JSON text |
| 2 | [`02-python-server-cpp-client`](02-python-server-cpp-client) | REQ client | REP server | UTF-8 JSON text |
| 3 | [`03-json-nlohmann`](03-json-nlohmann) | REP server with `nlohmann::json`, validation, error replies | REQ client | structured JSON |
| 4 | [`04-binary-pubsub`](04-binary-pubsub) | PUB, 100 Hz, packed `struct` + `float[6]` | SUB with `struct` + `numpy.frombuffer` | binary multipart |

## Installation (Ubuntu)

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libzmq3-dev cppzmq-dev nlohmann-json3-dev python3 python3-venv
git clone https://github.com/Michdo93/ZeroMQ-Cross-Language-CPP-Python.git
cd ZeroMQ-Cross-Language-CPP-Python
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
```

On older Ubuntu releases the cppzmq header package is called `libcppzmq-dev`. Check with `pkg-config --modversion libzmq` and `ls /usr/include/zmq.hpp`.

Build all C++ programs:

```bash
for d in 0*/; do cmake -S "$d" -B "build/$d" && cmake --build "build/$d"; done
```

## 1. C++ server + Python client

```bash
./build/01-cpp-server-python-client/server          # Terminal A
python 01-cpp-server-python-client/client.py        # Terminal B
# [Python client] reply: {'ok': True, 'reply': 'Hello from C++'}
```

## 2. Python server + C++ client

```bash
python 02-python-server-cpp-client/server.py        # Terminal A
./build/02-python-server-cpp-client/client          # Terminal B
# [C++ client] reply: {"ok": true, "reply": "Hello from Python"}
```

## 3. Structured JSON with nlohmann/json

```bash
./build/03-json-nlohmann/server                     # Terminal A (port 5591)
python 03-json-nlohmann/client.py                   # Terminal B
```

```
{'count': 5, 'mean': 21.86, 'stddev': 0.75, 'min': 20.8, 'max': 23.1, 'ok': True, ...}
{'error': 'values must not be empty', 'ok': False}
{'error': '[json.exception.type_error.302] type must be array, but is string', 'ok': False}
{'error': 'unknown command: reboot', 'ok': False}
```

Wrong input never crashes the server: every `json::exception` becomes an error reply (a REP socket must always answer).

## 4. Binary multipart PUB/SUB

```bash
python 04-binary-pubsub/subscriber.py 200           # Terminal A, stops after 200 messages
./build/04-binary-pubsub/publisher                  # Terminal B (Ctrl+C to stop)
# received 200, lost 0, median transport delay ~400 us
```

Frames: `["imu.raw", header(<IQ, 12 bytes), samples(<f4 x 6, 24 bytes)]`. The C++ struct is declared with `#pragma pack(1)` and a `static_assert`; Python decodes it with `struct.Struct("<IQ")` and `numpy.frombuffer(..., dtype="<f4")`. Byte order (`<` = little endian) and layout must match exactly. For evolving schemas prefer MessagePack, Protocol Buffers or FlatBuffers.

## Configuration

All programs read `ZMQ_ENDPOINT` (defaults: 5590 for 1/2, 5591 for 3, 5592 for 4). Set the same value in both terminals, e.g. `export ZMQ_ENDPOINT=tcp://127.0.0.1:6000`. For other hosts bind the server to `tcp://*:PORT`, connect the client to `tcp://SERVER_IP:PORT` and add CURVE (see ZeroMQ-Advanced-Examples).

## Pitfalls

* `zmq::poll()` throws `zmq::error_t` with `EINTR` when a signal arrives – catch it (see `server.cpp`), otherwise Ctrl+C ends in `terminate called after throwing ...`.
* `recv()` in cppzmq returns `std::optional<size_t>`; an empty optional means timeout (`rcvtimeo`).
* `send_string()`/`recv_string()` in Python are UTF-8; C++ sees raw bytes – there is no hidden length prefix or terminator.
* Use `sndmore` in C++ / `send_multipart()` in Python for multipart messages; a frame is delivered atomically together with all its siblings.
