import json,os,zmq
ctx=zmq.Context();s=ctx.socket(zmq.REP);s.bind(os.getenv('ZMQ_ENDPOINT','tcp://127.0.0.1:5590'));print('REP server ready; Ctrl-C to stop')
try:
 while True:
  raw=s.recv();req=json.loads(raw.decode('utf-8'));print('received:',req);s.send_json({'ok':True,'reply':'Hello from Python'})
except KeyboardInterrupt:pass
finally:s.close(0);ctx.term()
