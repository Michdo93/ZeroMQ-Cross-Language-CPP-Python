import json,os,zmq
ctx=zmq.Context();s=ctx.socket(zmq.REQ);s.setsockopt(zmq.RCVTIMEO,3000);s.connect(os.getenv('ZMQ_ENDPOINT','tcp://127.0.0.1:5590'))
try:s.send_json({'message':'hello from Python'});print(s.recv_json())
finally:s.close(0);ctx.term()
