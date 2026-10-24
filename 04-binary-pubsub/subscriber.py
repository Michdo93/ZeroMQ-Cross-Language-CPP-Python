"""04: Python SUB decoding the binary multipart messages of the C++ publisher.

Requires numpy (pip install numpy).
"""

import os
import struct
import sys
import time

import numpy as np
import zmq

ENDPOINT = os.getenv("ZMQ_ENDPOINT", "tcp://127.0.0.1:5592")
HEADER = struct.Struct("<IQ")  # uint32 seq, uint64 timestamp_us, little endian, no padding


def main() -> None:
    limit = int(sys.argv[1]) if len(sys.argv) > 1 else 0
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.connect(ENDPOINT)
    socket.setsockopt(zmq.SUBSCRIBE, b"imu.")
    print(f"[Python SUB] listening on {ENDPOINT}")

    received, lost, last_seq, delays = 0, 0, None, []
    try:
        while limit == 0 or received < limit:
            topic, header, payload = socket.recv_multipart()
            seq, ts_us = HEADER.unpack(header)
            samples = np.frombuffer(payload, dtype="<f4")  # zero-copy view, 6 floats
            delays.append(time.time() * 1e6 - ts_us)
            if last_seq is not None and seq != last_seq + 1:
                lost += seq - last_seq - 1  # PUB/SUB drops instead of blocking
            last_seq = seq
            received += 1
            if seq % 50 == 0:
                print(f"{topic.decode()} seq={seq:5d} acc={samples[:3].round(3)} gyro={samples[3:].round(3)}")
    except KeyboardInterrupt:
        pass
    finally:
        socket.close(linger=0)
        context.term()
    if delays:
        print(f"received {received}, lost {lost}, median transport delay {np.median(delays):.0f} us")


if __name__ == "__main__":
    main()
