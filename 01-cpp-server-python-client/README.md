# 01-cpp-server-python-client

This folder is part of the parent repository. Read the repository root README for the full learning path and security guidance.

Install the C++ toolchain, libzmq and cppzmq headers, plus Python and PyZMQ as described in the root README. Build with CMake from the repository root:

```bash
cmake -S 01-cpp-server-python-client -B build/01-cpp-server-python-client
cmake --build build/01-cpp-server-python-client
```

Start the server executable or Python server in one terminal, then run the paired client in another terminal. Both sides use `ZMQ_ENDPOINT` or default to `tcp://127.0.0.1:5590`.
