
import cv2
import numpy as np
from time import sleep
from picamera2 import Picamera2

import sys
import os

filename = "test.txt"

cv2.startWindowThread()

height = 640
width = 480

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (height, width)}))
picam2.start()

with open(filename,'w') as f:

    while True:
        im = picam2.capture_array()
        
        markerImage = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)

        aru_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
        
        parameters = cv2.aruco.DetectorParameters()
        
        detector = cv2.aruco.ArucoDetector(aru_dict,parameters)

        corners,ids,rejected = detector.detectMarkers(markerImage)

        if ids is not None:
            id = ids[0][0]
            print("id",id)
        else :
            id = 0

        # if ids is not None:
        #     cv2.aruco.drawDetectedMarkers(markerImage,corners,ids)

        # cv2.imshow("test",markerImage)
        
        img_hsv=cv2.cvtColor(im, cv2.COLOR_BGR2HSV)

        lower_red = np.array([0,50,50]) 
        upper_red = np.array([40,255,255])
        mask = cv2.inRange(img_hsv, lower_red, upper_red)

        # img_gray=cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
        # th_value = 100
        # _, mask = cv2.threshold(img_gray,th_value,255,cv2.THRESH_BINARY)
        

        red_pixels = cv2.countNonZero(mask)
        total_pixels = mask.size
        red_percentage = (red_pixels / total_pixels) * 100

        # Default state
        state = "None"

        # Check if red is dominant

            
        if True:
            contours, _ = cv2.findContours(mask,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
            
            if contours:
                largest_contour = max(contours, key=cv2.contourArea)

                M = cv2.moments(largest_contour)

                [vx,vy,x,y] = cv2.fitLine(largest_contour,cv2.DIST_L2,0,0.01,0.01)

                d = vy/vx

                state = "left" if d[0] > 0 else "right"
                
                # if 0.3 > d[0]  and d[0]>-0.3:
                #     state = " go"

                if M['m00'] > 0:
                    cx = int(M['m10']/M['m00'])
                    cy = int(M['m01']/M['m00'])

                    cv2.circle(im,(cx,cy),50,(0,255,0),-1)
                    cv2.putText(im,f"state : {state}",(cx+20,cy-10),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255),1)

                # print(d)
        
        cv2.imshow("mask",mask)
        cv2.imshow("frame",im)
        cv2.waitKey(1)
#        cv2.imshow("mask",mask)
        f.write(state + str(id)+"\n")
        f.flush()
       # sleep(0.001)

