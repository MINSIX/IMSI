import cv2
import numpy as np
from picamera2 import Picamera2

# 결과를 저장할 파일 이름
filename = "test.txt"

# 카메라 설정
height = 640
width = 480
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (height, width)}))
picam2.start()

# 파일 열기

while True:
    # 카메라에서 이미지 캡처
    im = picam2.capture_array()


    aru_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
    parameters = cv2.aruco.DetectorParameters()
    detector = cv2.aruco.ArucoDetector(aru_dict, parameters)


    # 마커 감지 (Aruco)
    markerImage_RGB = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)
    corners_RGB, ids_RGB, rejected_RGB = detector.detectMarkers(markerImage_RGB)

    markerImage_GRAY = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    corners_GRAY, ids_GRAY, rejected_GRAY = detector.detectMarkers(markerImage_GRAY)


    corners, ids, rejected = detector.detectMarkers(im)
    
    lower_blue = (0, 0, 0) # hsv 이미지에서 바이너리 이미지로 생성 , 적당한 값 30
    upper_blue = (80, 80, 80)
    marker_mask = cv2.inRange(markerImage_RGB, lower_blue, upper_blue) # 범위내의 픽셀들은 흰색, 나머지 검은색


    corners_mask, ids_mask, rejected_mask = detector.detectMarkers(marker_mask)


    marker_id = 0  # 기본값: 0
    
    marker_id=[i for i in [ids_RGB,ids_GRAY,ids,ids_mask] if i is not None]
    # 감지된 마커 ID

    if marker_id is not None and marker_id:
        
        marker_id = marker_id[0][0][0]

        print(marker_id)
    if marker_id is not None:
            cv2.aruco.drawDetectedMarkers(markerImage_GRAY,corners_GRAY,ids_GRAY)

    cv2.imshow("test",markerImage_GRAY)

    # HSV 색상 공간으로 변환
    img_hsv = cv2.cvtColor(im, cv2.COLOR_BGR2HSV)

    # 빨간색 범위 설정
    lower_blue = (0, 50, 50) # hsv 이미지에서 바이너리 이미지로 생성 , 적당한 값 30
    upper_blue = (25, 255, 255)
    img_mask = cv2.inRange(img_hsv, lower_blue, upper_blue) # 범위내의 픽셀들은 흰색, 나머지 검은색
    lower_blue = (155, 50, 50) # hsv 이미지에서 바이너리 이미지로 생성 , 적당한 값 30
    upper_blue = (180, 255, 255)
    img_mask2 = cv2.inRange(img_hsv, lower_blue, upper_blue)

    mask = cv2.bitwise_or(img_mask,img_mask2)
    # 화면을 왼쪽과 오른쪽으로 나누기
    height, width = mask.shape
    mask_1 = mask[:height // 3, :width // 3] 
    mask_2 = mask[:height // 3, width // 3:2*width // 3]  
    mask_3 = mask[:height // 3, 2*width // 3:] 
    mask_4 = mask[height // 3: 2* height // 3,  :width // 3]
    mask_5 = mask[height // 3: 2* height // 3,width // 3:2*width // 3]  
    mask_6 = mask[height // 3: 2* height // 3,2*width // 3:] 
    mask_7 = mask[2* height // 3: ,  :width // 3]
    mask_8 = mask[2* height // 3: ,width // 3:2*width // 3]  
    mask_9 = mask[2* height // 3: ,2*width // 3:] 
    # 각 영역에서 빨간색 픽셀 비율 계산
    red_1 = cv2.countNonZero(mask_1)
    red_2 = cv2.countNonZero(mask_2)
    red_3 = cv2.countNonZero(mask_3)
    red_4 = cv2.countNonZero(mask_4)
    red_5 = cv2.countNonZero(mask_5)
    red_6 = cv2.countNonZero(mask_6)
    red_7 = cv2.countNonZero(mask_7)
    red_8 = cv2.countNonZero(mask_8)
    red_9 = cv2.countNonZero(mask_9)

    size = height*width/9

    red_1_percentage = (red_1 / size) * 100
    red_2_percentage = (red_2 / size) * 100
    red_3_percentage = (red_3 / size) * 100
    red_4_percentage = (red_4 / size) * 100
    red_5_percentage = (red_5 / size) * 100
    red_6_percentage = (red_6 / size) * 100
    red_7_percentage = (red_7 / size) * 100
    red_8_percentage = (red_8 / size) * 100
    red_9_percentage = (red_9 / size) * 100

    left_wing = [red_1_percentage, red_4_percentage, red_7_percentage]
    right_wing = [red_3_percentage, red_6_percentage, red_9_percentage]
    front = [red_1_percentage, red_2_percentage, red_3_percentage]
    mid = [red_2_percentage, red_5_percentage, red_8_percentage]
    # 상태 결정
    state = "None"
    # if abs(left_percentage - right_percentage) < 3:  # 빨간색 비율 차이가 10% 이하라면
        # state = "go"
    if sum(front) < 10:
        front = "no"
    else:
        front = "yes"
    if  sum(left_wing)> sum(right_wing) :
        state = "left"
    else:
        state = "right"
    

    
    lower_blue = (50, 50, 50) # hsv 이미지에서 바이너리 이미지로 생성 , 적당한 값 30
    upper_blue = (120, 255, 255)
    img_mask3 = cv2.inRange(img_hsv, lower_blue, upper_blue) # 범위내의 픽셀들은 흰색, 나머지 검은색
    
    stop = cv2.countNonZero(img_mask3)
    front = "go"
    if (stop / size) * 100 > 60:
        front="stop"
        print("stop")
    
    with open(filename, 'w') as f:
    # 결과를 파일에 저장
        f.write(f"{front+state} {marker_id}\n")
        f.flush()  # 파일 내용을 즉시 디스크에 기록

    # 결과 화면에 표시
    cv2.putText(im, f"State: {front+state}", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.putText(im, f"Marker ID: {marker_id}", (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

    # 마스크 및 원본 이미지 표시
    cv2.imshow("Mask", img_mask3)
    cv2.imshow("Frame", im)

    # 종료 조건
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 종료 후 자원 해제
cv2.destroyAllWindows()
picam2.stop()
