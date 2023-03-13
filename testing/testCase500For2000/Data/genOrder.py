import random
import json
import string


with open(f'0.json', 'w') as f:
        x = dict()
        x["orders"] = [y for y in range(500, 399, -1)] ### for delete picture [500 to 400]
        x["orders"] = x["orders"] + [y for y in range(100, 0, -1)] ### update PICTURE [100 to 0]
        x["orders"] = x["orders"] + [y for y in range(2000, 2100)]
        json.dump(x, f, indent=4)