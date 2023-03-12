import random
import json
import string


#http://localhost:8080/api/messages/

amountUUIDBYTE = 36


def generate_random_string(length):
    letters = string.ascii_letters + string.digits
    # print(letters)
    return ''.join(random.choice(letters) for i in range(length))

uuidStore = []



uuidStore.append("")
for i in range(1,2000):
    data = dict()

    data["action"] = 0
    data["uuid"]   = (amountUUIDBYTE - len(str(i)))*"0" + str(i)
    uuidStore.append(data["uuid"])
    data["author"] = generate_random_string(64)
    data["message"]= generate_random_string(1024)
    data["likes"]  = 100
    data[ "imageUpdate"] = False
    if (i % 2 == 0):
        data[ "imageUpdate"] = True
        data["image"]  = generate_random_string(1 << 24)


    with open(f'{i}.json', 'w') as f:
        json.dump(data, f, indent=4)

##### put
for i in range(1900, 2000):
    data = dict()

    data["action"] = 1
    data["uuid"  ] = uuidStore[i]
    data["author"] = "tanawin"
    data["message"] = generate_random_string(1024)
    data["likes"]  = 256
    data[ "imageUpdate"] = True
    with open(f'{i-1900 + 2000}.json', 'w') as f:
        json.dump(data, f, indent=4)


##### delete
for i in range(1800, 1900):
    data["action"] = 2
    data["uuid"  ] = uuidStore[i]
    data["author"] = "yuiyui"
    data["message"]= generate_random_string(1024)
    data["likes"]  = 256
    data[ "imageUpdate"] = False

    with open(f'{i-1800 + 2100}.json', 'w') as f:
        json.dump(data, f, indent=4)
