"""03: Python client sending structured JSON to the C++ server."""

import os

import zmq

ENDPOINT = os.getenv("ZMQ_ENDPOINT", "tcp://127.0.0.1:5591")


def call(socket: zmq.Socket, payload: dict) -> dict:
    socket.send_json(payload)  # dict -> UTF-8 JSON bytes
    return socket.recv_json()  # UTF-8 JSON bytes -> dict


def main() -> None:
    context = zmq.Context()
    socket = context.socket(zmq.REQ)
    socket.setsockopt(zmq.RCVTIMEO, 3000)
    socket.setsockopt(zmq.LINGER, 0)
    socket.connect(ENDPOINT)
    try:
        print(call(socket, {"command": "stats", "values": [21.5, 22.0, 20.8, 23.1, 21.9]}))
        print(call(socket, {"command": "stats", "values": []}))
        print(call(socket, {"command": "stats", "values": "not a list"}))
        print(call(socket, {"command": "reboot"}))
    except zmq.Again:
        print("timeout - is the C++ server running?")
    finally:
        socket.close()
        context.term()


if __name__ == "__main__":
    main()
