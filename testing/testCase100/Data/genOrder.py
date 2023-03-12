import random
import json
import string


with open(f'0.json', 'w') as f:
        x = dict()
        x["orders"] = [y for y in range(1, 120)]
        json.dump(x, f, indent=4)