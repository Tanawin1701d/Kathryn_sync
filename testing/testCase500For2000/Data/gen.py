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
for i in range(2000,2100):
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



##### put with all image
for i in range(100, 0, -1):
    data = dict()

    data["action"] = 1
    data["uuid"  ] = (amountUUIDBYTE - len(str(i)))*"0" + str(i)
    data["author"] = "tanawin"
    data["message"] = generate_random_string(1024)
    data["likes"]  = 256
    data[ "imageUpdate"] = True
    data["image"]  = generate_random_string(1 << 24)
    with open(f'{i}.json', 'w') as f:
        json.dump(data, f, indent=4)


##### delete image
for i in range(500, 399, -1):
    data["action"] = 1
    data["uuid"  ] = (amountUUIDBYTE - len(str(i)))*"0" + str(i)
    data["author"] = "yuiyui"
    data["message"]= generate_random_string(1024)
    data["likes"]  = 256
    data[ "imageUpdate"] = False
    if (i % 2 == 0):
        data[ "imageUpdate"] = True

    with open(f'{i}.json', 'w') as f:
        json.dump(data, f, indent=4)
