
import cv2
import numpy as np
from time import sleep
from picamera2 import Picamera2

import sys
import os




cv2.startWindowThread()

height = 640
width = 480

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (height, width)}))
picam2.start()

while True:
    im = picam2.capture_array()
    im=cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    cv2.imshow("image",im)
    aru_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
    
    parameters = cv2.aruco.DetectorParameters()
    
    detector = cv2.aruco.ArucoDetector(aru_dict,parameters)

    corners,ids,rejected = detector.detectMarkers(im)
    print(4)
    if ids is not None:
        cv2.aruco.drawDetectedMarkers(im,corners,ids)

    cv2.imshow("test",im)

    cv2.waitKey(1)

