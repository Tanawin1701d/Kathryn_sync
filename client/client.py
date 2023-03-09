import requests
import msg_pb2
import numpy as np

dataDt = np.dtype({'names': ['uuid', 'author', 'message', 'likes', 'image'],
               'formats': ['U36', 'U64', 'U1024', np.int32, object]})
msgDt = np.dtype({'names': ['uuid', 'author', 'message', 'likes', 'image', 'needFeed', 'needImage', 'isDelete', 'isImageDelete'],
               'formats': ['U36', 'U64', 'U1024', np.int32, object, bool, bool, bool, bool]})

# this function is used to merge existing data with new data inspired from merge sort algorithm
def merge(arr, newArr):
    n1 = arr.shape[0]
    n2 = newArr.shape[0]
    resultArr = np.array([], dtype=dataDt)
    # Merge arr and newArr, return resultArr
    i = 0     # Initial index of arr
    j = 0     # Initial index of newArr
 
    while i < n1 and j < n2:
        if arr[i]['uuid'] < newArr[j]['uuid']:
            resultArr = np.append(resultArr, arr[i])
            i += 1
        elif arr[i]['uuid'] == newArr[j]['uuid']:
            if newArr[j]['isDelete']:
                i += 1
                j += 1
            else:
                if newArr[j]['needImage']:
                    arr[i]['image'] = newArr[j]['image']
                if newArr[j]['isImageDelete']:
                    arr[i]['image'] = ''
                if newArr[j]['needFeed']:
                    arr[i]['author'] = newArr[j]['author']
                    arr[i]['message'] = newArr[j]['message']
                    arr[i]['likes'] = newArr[j]['likes']
                resultArr = np.append(resultArr, arr[i])
                i += 1
                j += 1
        else:
            newDataArr = np.array([(newArr[j]['uuid'], newArr[j]['author'], newArr[j]['message'], newArr[j]['likes'], newArr[j]['image'])], dtype=dataDt)
            if newArr[j]['isImageDelete']:
                newDataArr[0]['image'] = ''
            if not newArr[j]['isDelete']:
                resultArr = np.concatenate((resultArr, newDataArr))
            j += 1

    # Copy the remaining elements of arr, if there
    # are any
    while i < n1:
        resultArr = np.append(resultArr, arr[i])
        i += 1
 
    # Copy the remaining elements of newArr, if there
    # are any
    while j < n2:
        remainArr = np.array([(newArr[j]['uuid'], newArr[j]['author'], newArr[j]['message'], newArr[j]['likes'], newArr[j]['image'])], dtype=dataDt)
        if newArr[j]['isImageDelete']:
            remainArr[0]['image'] = ''
        if not newArr[j]['isDelete']:
            resultArr = np.concatenate((resultArr, remainArr))
        j += 1
    
    return resultArr


# let the use input the server url
url = input('please enter url: ')
print(url)

# fetch new data from server until there isn't any new data left
response = requests.get(url+'/api/messages')
body = response.content
newArr = np.array([], dtype=msgDt)
while(body):
    readMsg = msg_pb2.msg()
    readMsg.ParseFromString(body)
    readMsgArr = np.array( [(readMsg.uuid, readMsg.author, readMsg.message, readMsg.likes, readMsg.image, readMsg.needFeed, readMsg.needImage, readMsg.isDelete, readMsg.isImageDelete)], dtype=msgDt)
    newArr = np.concatenate((newArr, readMsgArr))
    response = requests.get(url+'/api/messages')
    body = response.content

# create new file if it does not exist
with open('result.csv', 'a', newline='') as csvfile:
    pass

# read existing result from result.csv
with open('result.csv', 'r', newline='') as csvfile:
    try: 
        arr = np.loadtxt("result.csv",
                 delimiter=",", dtype=dataDt, usecols=(0, 1, 2, 3, 4))
    except Exception as e:
        arr = np.array([], dtype=dataDt)
    
    # this is for testing only please leave these lines commented
    # newArr = np.array([('122222', 'a1', 'm1', 44, 'img1', True, True, False, False),
    #                    ('3131', 'a1', 'm1', 44, 'img1', True, True, True, False),
    #                    ('544444', 'a54444', 'm1', 44, 'img54', True, False, False, True),
    #                    ('999', 'a1', 'm1', 44, 'img1', True, True, False, False)], dtype=msgDt)

    resultArray = merge(arr, newArr)
    print('--------result is--------')
    print(resultArray)
    np.savetxt('result.csv', resultArray, delimiter=',', fmt="%s,%s,%s,%s,%s")

    

