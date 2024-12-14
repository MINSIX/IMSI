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

    # 마커 감지 (Aruco)
    markerImage = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    aru_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
    parameters = cv2.aruco.DetectorParameters()
    detector = cv2.aruco.ArucoDetector(aru_dict, parameters)
    corners, ids, rejected = detector.detectMarkers(markerImage)

    # 감지된 마커 ID
    marker_id = 0  # 기본값: 0
    if ids is not None:
        marker_id = ids[0][0]

    # HSV 색상 공간으로 변환
    img_hsv = cv2.cvtColor(im, cv2.COLOR_BGR2HSV)

    # 빨간색 범위 설정
    lower_red = np.array([0, 50, 50])
    upper_red = np.array([40, 255, 255])
    mask = cv2.inRange(img_hsv, lower_red, upper_red)

    # 화면을 왼쪽과 오른쪽으로 나누기
    height, width = mask.shape
    left_mask = mask[:, :width // 2]  # 왼쪽 절반
    right_mask = mask[:, width // 2:]  # 오른쪽 절반

    # 각 영역에서 빨간색 픽셀 비율 계산
    left_red = cv2.countNonZero(left_mask)
    right_red = cv2.countNonZero(right_mask)

    total_left = left_mask.size
    total_right = right_mask.size

    left_percentage = (left_red / total_left) * 100
    right_percentage = (right_red / total_right) * 100

    # 상태 결정
    state = "None"
    # if abs(left_percentage - right_percentage) < 3:  # 빨간색 비율 차이가 10% 이하라면
        # state = "go"
    if left_percentage > right_percentage:
        state = "left"
    else:
        state = "right"
    with open(filename, 'w') as f:
    # 결과를 파일에 저장
        f.write(f"{state} {marker_id}\n")
        f.flush()  # 파일 내용을 즉시 디스크에 기록

    # 결과 화면에 표시
    cv2.putText(im, f"State: {state}", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.putText(im, f"Marker ID: {marker_id}", (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.putText(im, f"Left: {left_percentage:.2f}%", (50, 150), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
    cv2.putText(im, f"Right: {right_percentage:.2f}%", (50, 200), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

    # 마스크 및 원본 이미지 표시
    cv2.imshow("Mask", mask)
    cv2.imshow("Frame", im)

    # 종료 조건
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 종료 후 자원 해제
cv2.destroyAllWindows()
picam2.stop()
