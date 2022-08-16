import shutil
from pathlib import Path
import sys, tty, termios
from .formatting import color


def default_config():
    dst = Path().home()/'.reussrc'
    src = Path(__file__).parent/'reussrc.in'
    print(f'Copying: {src} to {dst}')
    shutil.copy(src, dst)
    print(color.yellow("Adapt ~/.reussrc to your detector before proceeding"))


def getch():
    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)
    new = termios.tcgetattr(fd)
    new[3] = new[3] & ~termios.ECHO
    new[3] = new[3] & ~termios.ICANON
    try:
        termios.tcsetattr(fd, termios.TCSADRAIN, new)
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)
    return ch
