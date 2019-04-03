from ctypes import *
import math
import random
import os
import cv2
import numpy as np
import time
import darknet
import glob
import ntpath

def convertBack(x, y, w, h):
    xmin = int(round(x - (w / 2)))
    xmax = int(round(x + (w / 2)))
    ymin = int(round(y - (h / 2)))
    ymax = int(round(y + (h / 2)))
    return xmin, ymin, xmax, ymax


imgs = []
names = []

def cvDrawBoxes(detections, img):
    global imgs, names
    for detection in detections:
        x, y, w, h = detection[2][0],\
            detection[2][1],\
            detection[2][2],\
            detection[2][3]

        #factor_x = 3.9135
        #factor_y = 2.9712
        factor_x = 2.677631579
        factor_y = 2.032894737
        #factor_x = 1.956730769
        #factor_y = 1.485576923

        x = int(float(x) * factor_x) # 1628
        y = int(float(y) * factor_y) # 1236
        w = int(float(w) * factor_x)
        h = int(float(h) * factor_y)
        xmin, ymin, xmax, ymax = convertBack(
            float(x), float(y), float(w), float(h))
        pt1 = (xmin, ymin)
        pt2 = (xmax, ymax)
        cv2.rectangle(img, pt1, pt2, (0, 255, 0), 2)

        idx = 0
        for name in names:
            if detection[0].decode() == name:
                sign = imgs[idx]
                
                #toprint = cv2.resize(sign, (50,50))
                #img[50:100, 50:100] = toprint

                sign_h = sign.shape[0]
                sign_w = sign.shape[1]

                if w < sign_w and h < sign_h:
                    sign_h = h
                    sign_w = w

                sign = cv2.resize(sign, (int(sign_w), int(sign_h)))
                x = int(x) - int(w)/2 - sign.shape[1]
                y = int(y) - int(h)/2
                if x < 0:
                    x = int(x) + 2 * int(w)
                if y < 0:
                    y = int(y) + int(h)
                try:
                    img[int(y):int(y)+sign.shape[0], int(x):int(x)+sign.shape[1]] = sign
                except:
                    print('error')
            idx = idx + 1

        #cv2.putText(img,
        #            detection[0].decode() +
        #            " [" + str(round(detection[1] * 100, 2)) + "]",
        #            (pt1[0], pt1[1] - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
        #            [0, 255, 0], 2)
    return img


netMain = None
metaMain = None
altNames = None


def YOLO():

    global imgs, names
    paths = glob.glob("signs/*.png")
    for path in paths:
        img = cv2.imread(path, cv2.IMREAD_UNCHANGED)
        imgs.append(cv2.cvtColor(img, cv2.COLOR_BGRA2RGB))
        names.append(os.path.splitext(ntpath.basename(path))[0])


    global metaMain, netMain, altNames
    configPath = "./yolo-train.cfg"
    weightPath = "./weights.weights"
    metaPath = "./obj.data"
    if not os.path.exists(configPath):
        raise ValueError("Invalid config path `" +
                         os.path.abspath(configPath)+"`")
    if not os.path.exists(weightPath):
        raise ValueError("Invalid weight path `" +
                         os.path.abspath(weightPath)+"`")
    if not os.path.exists(metaPath):
        raise ValueError("Invalid data file path `" +
                         os.path.abspath(metaPath)+"`")
    if netMain is None:
        netMain = darknet.load_net_custom(configPath.encode(
            "ascii"), weightPath.encode("ascii"), 0, 1)  # batch size = 1
    if metaMain is None:
        metaMain = darknet.load_meta(metaPath.encode("ascii"))
    if altNames is None:
        try:
            with open(metaPath) as metaFH:
                metaContents = metaFH.read()
                import re
                match = re.search("names *= *(.*)$", metaContents,
                                  re.IGNORECASE | re.MULTILINE)
                if match:
                    result = match.group(1)
                else:
                    result = None
                try:
                    if os.path.exists(result):
                        with open(result) as namesFH:
                            namesList = namesFH.read().strip().split("\n")
                            altNames = [x.strip() for x in namesList]
                except TypeError:
                    pass
        except Exception:
            pass
    #cap = cv2.VideoCapture(0)
    cap = cv2.VideoCapture("test.mp4")
    cap.set(3, 1628)
    cap.set(4, 1236)
    out = cv2.VideoWriter(
        "output.avi", cv2.VideoWriter_fourcc(*"MJPG"), 10.0,
        (1628, 1236))
    print("Starting the YOLO loop...")

    # Create an image we reuse for each detect
    darknet_image = darknet.make_image(darknet.network_width(netMain),
                                    darknet.network_height(netMain),3)
    while True:
        prev_time = time.time()
        ret, frame_read = cap.read()
        frame_rgb = cv2.cvtColor(frame_read, cv2.COLOR_BGR2RGB)
        frame_resized = cv2.resize(frame_rgb,
                                   (darknet.network_width(netMain),
                                    darknet.network_height(netMain)),
                                    interpolation=cv2.INTER_LINEAR)

        darknet.copy_image_from_bytes(darknet_image,frame_resized.tobytes())

        detections = darknet.detect_image(netMain, metaMain, darknet_image, thresh=0.25)
        image = cvDrawBoxes(detections, frame_rgb)

        #overlay = image.copy()
        #cv2.rectangle(overlay, (0, 0), (1628, 200), (0, 0, 0), -1)
        #image = cv2.addWeighted(overlay, 0.5, image, 1 - 0.5, 0)

        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        print(1/(time.time()-prev_time))
        out.write(image)
        cv2.imshow('Demo', image)
        cv2.waitKey(3)
    cap.release()
    out.release()

if __name__ == "__main__":
    YOLO()
