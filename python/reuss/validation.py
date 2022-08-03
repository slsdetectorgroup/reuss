

import json
from pathlib import Path
# Valid json
# - a string
# - a number
# - an object (JSON object)
# - an array
# - a boolean
# - null

def json_string(values : dict) -> str:
    for key, item in values.items():
        if isinstance(item, Path):
            values[key] = str(item)
    return json.dumps(values)