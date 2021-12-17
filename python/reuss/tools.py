import shutil
from pathlib import Path

from .formatting import color
def default_config():
    dst = Path().home()/'.reussrc'
    src = Path(__file__).parent/'reussrc.in'
    print(f'Copying: {src} to {dst}')
    shutil.copy(src, dst)
    print(color.yellow("Adapt ~/.reussrc to your detector before proceeding"))