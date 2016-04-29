#!/usr/bin/python
__author__ = 'ar'

import numpy as np
from skimage.feature import daisy
from skimage import data
import matplotlib.pyplot as plt

if __name__=='__main__':
    img = data.camera()
    descs, descs_img = daisy(img, step=180, radius=58, rings=2, histograms=6, orientations=8, visualize=True)
    print descs.shape

    fig, ax = plt.subplots()
    ax.axis('off')
    ax.imshow(descs_img)
    descs_num = descs.shape[0] * descs.shape[1]
    ax.set_title('%i DAISY descriptors extracted:' % descs_num)
    plt.show()