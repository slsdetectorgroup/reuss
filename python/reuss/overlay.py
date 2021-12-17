from . import config as cfg
import json #needed to decode shapes
import matplotlib.pyplot as plt
import numpy as np

def draw_overlay(ax):
    for key, value in cfg.parser['overlay'].items():
        s = json.loads(value)
        if 'circle' in key:
            func = plt.Circle
        elif 'rectangle' in key:
            func = plt.Rectangle
        else:
            raise ValueError("Only circle or rectangle is currently supported")
        obj = func(**s)
        ax.add_artist(obj)


if __name__ == "__main__":
    image = np.zeros((cfg.nrows(), cfg.ncols()))
    fig, ax = plt.subplots()
    im = ax.imshow(image, origin = cfg.plot.origin)
    draw_overlay(ax)
    plt.show()