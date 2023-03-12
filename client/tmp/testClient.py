import requests
import msg_pb2
import numpy as np
import shutil
import os
import argparse 
import struct


msgDt = np.dtype({'names': ['uuid', 'author', 'message', 'likes', 'image', 'needFeed', 'needImage', 'isDelete', 'isImageDelete'],
               'formats': ['U36', 'U64', 'U1024', np.int32, object, bool, bool, bool, bool]})


file  = open("resultProto", "rb")

readMsg = msg_pb2.msg()

binary_data =  file.read(4)
uint32_value = struct.unpack("<I", binary_data)[0]
stringer = file.read(uint32_value)

readMsg.ParseFromString(stringer)
print(type(stringer))
print(stringer)

readMsgArr = np.array( [(readMsg.uuid, readMsg.author, 
                            readMsg.message, readMsg.likes, 
                            readMsg.image, readMsg.needFeed, 
                            readMsg.needImage, readMsg.isDelete, 
                            readMsg.isImageDelete)], dtype=msgDt)


print(readMsgArr)

binary_data =  file.read(4)
uint32_value = struct.unpack("<I", binary_data)[0]
stringer = file.read(uint32_value)
print(type(stringer))
print(stringer)
readMsg.ParseFromString(stringer)

readMsgArr = np.array( [(readMsg.uuid, readMsg.author, 
                            readMsg.message, readMsg.likes, 
                            readMsg.image, readMsg.needFeed, 
                            readMsg.needImage, readMsg.isDelete, 
                            readMsg.isImageDelete)], dtype=msgDt)


print(readMsgArr)