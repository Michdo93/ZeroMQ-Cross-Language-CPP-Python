"""02: Python REP server answering the C++ REQ client.

The C++ client sends raw UTF-8 bytes; recv_json() simply decodes them.
"""

import os
import signal

import zmq

ENDPOINT = os.getenv("ZMQ_ENDPOINT", "tcp://127.0.0.1:5590")
running = True


def stop(*_):
    global running
    running = False


def main() -> None:
    signal.signal(signal.SIGINT, stop)
    signal.signal(signal.SIGTERM, stop)
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind(ENDPOINT)
    print(f"[Python server] REP on {ENDPOINT} (Ctrl+C to stop)")
    try:
        while running:
            if socket.poll(500):
                request = socket.recv_json()
                print("[Python server] received:", request)
                socket.send_json({"ok": True, "reply": "Hello from Python"})
    finally:
        socket.close(linger=0)
        context.term()


if __name__ == "__main__":
    main()
