import socket
import time
import datetime
import json
import os
import random

# connection
host = socket.gethostname()
port = 9999
print(host);
buffer_size = 1024

# time
time_epoch = time.time()
time_utc = time.gmtime(time_epoch)
time_local = time.localtime(time_epoch)

# ping
message_seqno = 1
message_version = 0.01
message_utc = time.strftime('%Y/%m/%d %H:%M:%S', time_utc)
message_local = time.strftime('%Y/%m/%d %H:%M:%S', time_local)
message_action = "ping"

# body
ping_id = 1

# connection
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((host, port))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

while True:
  #s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  #s.connect((host, port))

  message_json = '"message": { "message_seqno": "%015i", "message_version": %.2f, "message_utc": "%s", "message_local": "%s", "message_action": "%s" }'\
  %(message_seqno, message_version, message_utc, message_local, message_action)

  header_json = '"header": { %s }'%(message_json)

  body_json = '"body": { "ping_id": %i }'%(ping_id)

  payload_json = '{ %s, %s }'%(header_json, body_json)
  
  obj = json.loads(payload_json)
  formatted_json = json.dumps(obj, indent=2)
  print(formatted_json)
  
  payload_header = '%015i ' %(len(payload_json) + 16)
  send_buffer = b'' + payload_header + payload_json;
  print('send: ' + send_buffer)
  
  s.sendall(send_buffer)
  
  #time.sleep(100000000)
  
  data = s.recv(buffer_size)
  print('recieve: ', repr(data))
 
  #time.sleep(1)
 
  # update inputs
  message_seqno+=1
  ping_id+=1

  #time.sleep(1)

  #time.sleep(15)
  #s.close()

  if ping_id == 1:
    break

s.close()
