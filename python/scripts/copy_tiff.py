import shutil
from pathlib import Path
import os
import jungfrau as jf
base = Path('/fast_raid0_md0/May')

folders = [f for f in os.listdir(base) if f.startswith('0')]


for f in folders:
    jf.makedirs(base/'tiff'/f)
    for img in os.listdir(base/f/'tiff'):
        src = base/f/'tiff'/img
        dst = base/'tiff'/f/img
        print(src, '->', dst)
        shutil.copy(src, dst)

