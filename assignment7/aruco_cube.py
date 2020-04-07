import cv2
import numpy as np
from PIL import Image
import ffmpy
import sys
import os
import math
import time
import functools
from cv2 import aruco
import pickle


# Arcu Choice
aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250)

# For drawing axis
marker_length = 100
axis = np.float32([[0,0,0], [0,marker_length,0], [marker_length,marker_length,0], [marker_length,0,0],
                   [0,0,marker_length],[0,marker_length,marker_length],[marker_length,marker_length,marker_length],[marker_length,0,marker_length] ])

# Load Prior Configuration Data
ret, mtx, dist, rvecs, tvecs = pickle.load(open("cal_data.pkl", "rb"))

def draw_cube(img, imgpts):
    imgpts = np.int32(imgpts).reshape(-1,2)

    # draw ground floor in green
    img = cv2.drawContours(img, [imgpts[:4]],-1,(0,255,0),-3)

    # draw pillars in blue color
    for i,j in zip(range(4),range(4,8)):
        img = cv2.line(img, tuple(imgpts[i]), tuple(imgpts[j]),(255),3)

    # draw top layer in red color
    img = cv2.drawContours(img, [imgpts[4:]],-1,(0,0,255),3)

    return img


def get_frames(cap):
    # Generate Sample Video
    frames = []
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Can't receive frame (stream end?). Exiting ...")
            break
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        parameters =  aruco.DetectorParameters_create()
        corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)
        if ids != None:
            frame_markers = aruco.drawDetectedMarkers(frame, corners, ids)
            rvec, tvec, _ = aruco.estimatePoseSingleMarkers(corners, 100, mtx, dist)
            for i, marker in enumerate(ids):
                imgpts, jac = cv2.projectPoints(axis, rvec[i], tvec[i], mtx, dist)
                aruco.drawAxis(frame, mtx, dist, rvec[i], tvec[i], marker_length*10)
                draw_cube(frame, imgpts)
        frames.append(frame)
        # Display Frame
        if cv2.waitKey(1) == ord('q'):
            break
    return frames


def main():
    output_file = 'sample'
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    avi_file_name = f'{output_file}.avi'

    cap = cv2.VideoCapture("final_surface.mov")
    # Get the height and width
    w = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
    h = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)

    out = cv2.VideoWriter(avi_file_name, fourcc, 20.0, (int(w),int(h)))
    output_file_name = f'{output_file}.mp4'

    [out.write(frame) for frame in get_frames(cap)]
    out.release()
    cap.release()
    cv2.destroyAllWindows()

    print("Creating mp4\n\n")
    if os.path.exists(output_file_name):
        os.remove(output_file_name)
    ff = ffmpy.FFmpeg(
        inputs={avi_file_name: None},
        outputs={output_file_name: None}
    )
    ff.run()

if __name__ == "__main__":
    main()







        


    