# ZeroMQ Cross-Language: C++ and Python

Two C++/Python REQ/REP examples exchange UTF-8 JSON; socket pattern, endpoint and schema must match. Ubuntu/Debian:

## Install

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libzmq3-dev cppzmq-dev python3 python3-venv
python3 -m venv .venv && source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install pyzmq
```

If `cppzmq-dev` unavailable, install equivalent `libcppzmq-dev`. Check discovery with `pkg-config --modversion libzmq`. CMake requires C++17.

## 1. C++ server + Python client

Terminal A, from root:

```bash
cmake -S 01-cpp-server-python-client -B build/cpp-server
cmake --build build/cpp-server
./build/cpp-server/server
```

Terminal B: `source .venv/bin/activate && python 01-cpp-server-python-client/client.py`

Python sends JSON; C++ prints received bytes and replies. Ctrl+C stops server.

## 2. Python server + C++ client

Terminal A: `source .venv/bin/activate && python 02-python-server-cpp-client/server.py`

Terminal B:

```bash
cmake -S 02-python-server-cpp-client -B build/cpp-client
cmake --build build/cpp-client
./build/cpp-client/client
```

C++ sends JSON; Python parses and responds. Ctrl+C stops server.

## Configure and customize

Default endpoint `tcp://127.0.0.1:5590`. Set `ZMQ_ENDPOINT` identically in both shells to change endpoint, e.g. `export ZMQ_ENDPOINT='tcp://127.0.0.1:5591'`. Both C++ and Python read this variable. For remote use, bind server to reachable interface, set client to server IP, configure firewall. Customize client JSON and update receiver as needed; keep one UTF-8 JSON value per message. Reply text is in server; C++ response is a string literal requiring escaped quotes.

## Troubleshooting/security

CMake cannot find libzmq: install `libzmq3-dev`, `pkg-config`, cppzmq headers and rerun. Missing `zmq.hpp`: install cppzmq headers. Port conflict: use another consistent endpoint. Timeout: start matching server and check address/firewall. Samples use plaintext TCP; use CURVE or encrypted tunnel, firewall and validate input before cross-host use.
