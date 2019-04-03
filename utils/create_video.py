import os
import cv2
import numpy as np
import glob


def create():
    #paths = glob.glob("./res_00/*")

    video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 15, (1628, 1236))

    for i in range(0, 3460):
    	img = cv2.imread('./res_00/frame_' + str(i) + '.jpg')
    	if img is not None:
    		video.write(img)
    	else:
    		print('error')

    video.release()

if __name__ == "__main__":
    create()
