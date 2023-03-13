from sys import byteorder
import requests
import msg_pb2
import numpy as np
import shutil
import os
import argparse 
import struct



parser = argparse.ArgumentParser("my argument")
parser.add_argument('-host'               , "--h"    , type = str , default =  "localhost" , help = "hostname")
parser.add_argument('-resultFile'         , "--r"    , type = str , default = "result.csv" , help = "number of iteration")
args   = parser.parse_args()

url = getattr(args, "h")
print("now program is targeting to ", url)


dataDt = np.dtype({'names'  : ['uuid', 'author', 'message', 'likes' , 'image'],
                   'formats': ['U36' , 'U64'   , 'U1024'  , np.int32, bool   ]})
msgDt = np.dtype({'names'  : ['uuid', 'author', 'message', 'likes' , 'image', 'needFeed', 'needImage', 'isDelete', 'isImageDelete'],
                  'formats': ['U36' , 'U64'   , 'U1024'  , np.int32, object , bool      , bool       , bool      , bool           ]})

#### jor data
jorData        = None
currentJorIter = 0
lastJorIter    = 0
####  stat
genereated     = 0
######################## write buffer to file
MAX_WRITE_BUFFER = 1000
writeBuffer      = np.array([], dtype=dataDt)
#############################################
#jor File
jorFile         = None
jorFileName     = "jor.csv"
#result File
resultFile      = None
resultName      = getattr(args, "r")

def tryFlush(forceFlush = False):
    global currentJorIter, lastJorIter, genereated, jorFileName, resultName, writeBuffer

    if (not forceFlush) and (writeBuffer.shape[0] < MAX_WRITE_BUFFER):
        return
        ### save to jor
    
    print("trying.......... flush")
    
    np.savetxt(jorFile, writeBuffer, fmt= "%s,%s,%s,%d,%r",header='uuid,author,message,likes,image')
    

    for daytaRow in writeBuffer:
    ### save to result
        resultFile.write(bytes('{},{},{},{},\n'.format(daytaRow['uuid'], 
                                                    daytaRow['author'], 
                                                    daytaRow['message'], 
                                                    str(daytaRow['likes']))
                              , 'utf-8'))
        if (daytaRow["image"]):
            imageFile = open("images/" + daytaRow['uuid'], "rb")
            shutil.copyfileobj(imageFile, resultFile)
            resultFile.write(b"\n")
            imageFile.close()

    writeBuffer = np.empty(0, dtype=dataDt)
    
    print("finished flush")
        
        



# this function is used to merge existing data with new data inspired from merge sort algorithm
def merge(newArr):  # it is msgDt
    global currentJorIter, lastJorIter, genereated, jorFileName, resultName, writeBuffer
    lastMsgIter = newArr.shape[0]
    currentMsgIter = 0
    #print(newArr)
    while( currentMsgIter < lastMsgIter ):
        
        ### for stat tracking
        #print(newArr[currentMsgIter]["uuid"], "   " ,newArr[currentMsgIter]["likes"], "   ", newArr[currentMsgIter]["author"])
        

        if ((currentJorIter == lastJorIter) or (jorData[currentJorIter]["uuid"] > newArr[currentMsgIter]["uuid"])):
            ######### case new message write to file
            needImage = False

            if newArr[currentMsgIter]["isDelete"]:
                currentMsgIter = currentMsgIter + 1
                continue
            elif not newArr[currentMsgIter]["isImageDelete"]:
                if newArr[currentMsgIter]["needImage"]:
                    needImage = True
                    imageFile = open( "images/" + newArr[currentMsgIter]["uuid"],'wb')
                    imageFile.write(newArr[currentMsgIter]["image"])
                    imageFile.close()

            preAppend=  np.array([(newArr[currentMsgIter]["uuid"],
                                   newArr[currentMsgIter]["author"],
                                   newArr[currentMsgIter]["message"],
                                   newArr[currentMsgIter]["likes"],
                                   needImage)], dtype=dataDt)
            writeBuffer = np.append(writeBuffer, preAppend)
            tryFlush()
            currentMsgIter = currentMsgIter + 1

        elif (jorData[currentJorIter]["uuid"] < newArr[currentMsgIter]["uuid"]):
            
            ######### case journal should write to file
            writeBuffer = np.append(writeBuffer, jorData[currentJorIter])
            tryFlush()
            currentJorIter = currentJorIter + 1
        elif (jorData[currentJorIter]["uuid"] == newArr[currentMsgIter]["uuid"]):
            #### merge two file
            uuid = newArr[currentMsgIter]["uuid"]
            author  = None 
            message = None 
            likes   = None
            
            NeedImageDisk     = jorData[currentJorIter]["image"]
            NeedImageInternet = newArr[currentMsgIter]["needImage"]
            imageDeleteCmd    = newArr[currentMsgIter]['isImageDelete']
            
            if (newArr[currentMsgIter]["isDelete"]):
                currentJorIter  = currentJorIter + 1
                currentMsgIter = currentMsgIter  + 1
                continue
            if (newArr[currentMsgIter]['needFeed']):
                ### feed is need from internet
                author  = newArr[currentMsgIter]["author"]
                message = newArr[currentMsgIter]["message"]
                likes   = newArr[currentMsgIter]["likes"]
            else:
                ### feed is need from jornal
                author  = jorData[currentJorIter]["author"]
                message = jorData[currentJorIter]["message"]
                likes   = jorData[currentJorIter]["likes"]

            
            ### write image to file
            if NeedImageInternet:
                imageFile = open( "images/" + newArr[currentMsgIter]["uuid"],'wb')
                imageFile.write(newArr[currentMsgIter]["image"])
                imageFile.close()


            preAppend=  np.array([(uuid,
                                   author,
                                   message,
                                   likes,
                                   (not imageDeleteCmd) and (NeedImageDisk or NeedImageInternet)
                                   )], dtype=msgDt)
            writeBuffer = np.append(writeBuffer, preAppend)
            tryFlush()
            currentJorIter  = currentJorIter + 1
            currentMsgIter  = currentMsgIter + 1
             




# let the use input the server url



def getReq():
    global currentJorIter, lastJorIter, genereated, jorFileName, resultName

    while (True):
        # fetch new data from server until there isn't any new data left
        response = requests.get(url)
        print ("finished")
        body = np.frombuffer( response.content, dtype=np.uint8) # for now body is numpy array
        
    
        if (body.shape[0] == 0):
            break # in-case we know that it is exited

        decodedBatch = np.array([], dtype=msgDt)
        while(body.any()):
            #to check message size but (must check that at server is little or big endian)(for now we check client instead during demo)
            sizeOfMessage = struct.unpack("<I", body[:4])[0]
            readMsg = msg_pb2.msg()
                                        ###### size of message size indication is 4 byte
            readMsg.ParseFromString(body[4: 4 + sizeOfMessage])
            readMsgArr = np.array( [(readMsg.uuid, readMsg.author, 
                                     readMsg.message, readMsg.likes, 
                                     readMsg.image, readMsg.needFeed, 
                                     readMsg.needImage, readMsg.isDelete, 
                                     readMsg.isImageDelete)], dtype=msgDt)
            decodedBatch = np.append(decodedBatch, readMsgArr)
            body = body[4 + sizeOfMessage:]

            #print("uuid get ",readMsg.uuid)

        #### TODO make it multithread
        print("system merging @generated items =",genereated)
        merge(decodedBatch)
        #print("system finished merge @generated items =",genereated)

print(os.path.isfile(jorFileName))
if (os.path.isfile(jorFileName)):
    try: 
        # read existing jornal file .csv to
        jorData = np.loadtxt(jorFileName,
                 delimiter=",", dtype=dataDt, usecols=(0, 1, 2, 3, 4) )
        print("jor data with shape 0 ", jorData.shape[0])
        lastJorIter = jorData.shape[0]
    except Exception as e:
        jorData = np.array([], dtype=dataDt)


jorFile    = open(jorFileName, "w" )
resultFile = open(resultName , "wb", buffering=67108864)


getReq()

### last batch of merge for in jor
while currentJorIter < lastJorIter:
    writeBuffer = np.append(writeBuffer, jorData[currentJorIter])
    currentJorIter = currentJorIter + 1


tryFlush(True)
jorFile.close()
resultFile.close()
