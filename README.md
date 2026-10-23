# ZeroMQ Cross-Language: C++ and Python

Two interoperable REQ/REP examples using UTF-8 JSON. Both sides must agree on socket pattern, endpoint, framing, and message schema. Requires Ubuntu/Debian, C++17 compiler, CMake, libzmq, cppzmq headers, Python 3.10+, and PyZMQ.

## Install dependencies

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libzmq3-dev cppzmq-dev python3 python3-venv
python3 -m venv .venv && . .venv/bin/activate
python -m pip install --upgrade pip pyzmq
```

On distributions naming the header package `libcppzmq-dev`, install that package instead of `cppzmq-dev`.

## Example 1: C++ server, Python client

```bash
cmake -S 01-cpp-server-python-client -B build/cpp-server
cmake --build build/cpp-server
./build/cpp-server/server
# In another terminal, from the repo root:
. .venv/bin/activate && python 01-cpp-server-python-client/client.py
```

## Example 2: Python server, C++ client

```bash
python 02-python-server-cpp-client/server.py
# In another terminal:
cmake -S 02-python-server-cpp-client -B build/cpp-client
cmake --build build/cpp-client
./build/cpp-client/client
```

Both servers default to `tcp://127.0.0.1:5590` and accept `ZMQ_ENDPOINT` to override it. Ctrl+C stops a server. The sample JSON schema is `{ "message": "..." }`; the response contains `ok` and `reply`. These examples use plaintext TCP and loopback; add CURVE or a protected tunnel and firewall policy before using across hosts. Install matching libzmq development/runtime versions on both systems. Use `pkg-config --modversion libzmq` to inspect the installed version.
