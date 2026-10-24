"""01: Python REQ client talking to the C++ REP server."""

import os

import zmq

ENDPOINT = os.getenv("ZMQ_ENDPOINT", "tcp://127.0.0.1:5590")


def main() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REQ)
    socket.setsockopt(zmq.RCVTIMEO, 3000)
    socket.setsockopt(zmq.LINGER, 0)
    socket.connect(ENDPOINT)
    try:
        socket.send_json({"message": "hello from Python"})
        print("[Python client] reply:", socket.recv_json())
    except zmq.Again:
        print("[Python client] timeout - is the C++ server running?")
    finally:
        socket.close()
        context.term()


if __name__ == "__main__":
    main()
