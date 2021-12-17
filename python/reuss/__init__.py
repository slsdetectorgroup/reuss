

from typing import IO

# Guard again no config file found since running without
# is most likely not what you want 
found_config =  False

try:
    from . import config
    found_config = True
except IOError:
    print("reuss config file (~/.reussrc) not found\nUse reuss.default_config() to copy a template to your home folder")
    from .tools import default_config
if found_config:
    from _reuss import *
    from .DataCollector import DataCollector
    from .formatting import color
    from .calibration import load_calibration
    from . import io
    from . import shm



