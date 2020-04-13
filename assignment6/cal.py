import cv2
import numpy as np
from PIL import Image
import ffmpy
import sys
import os
import math
import time
import functools
import pickle

def get_corners(nx, ny, img):
    # Find the chessboard corners
    ret, corners = cv2.findChessboardCorners(img, (nx, ny), True)
    return([ret, corners])


def draw(img, corners, imgpts):
    corner = tuple(corners[0].ravel())
    img = cv2.line(img, corner, tuple(imgpts[0].ravel()), (255,0,0), 5)
    img = cv2.line(img, corner, tuple(imgpts[1].ravel()), (0,255,0), 5)
    img = cv2.line(img, corner, tuple(imgpts[2].ravel()), (0,0,255), 5)
    return img


def get_obj_points(x, y):
    objp = np.zeros((y*x,3), np.float32)
    objp[:,:2] =  np.mgrid[0:x,0:y].T.reshape(-1,2)
    return objp

criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
objp = np.zeros((7*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:7].T.reshape(-1,2)

# For drawing axis
axis = np.float32([[3,0,0], [0,3,0], [0,0,-3]]).reshape(-1,3)

# Load Prior Configuration Data
ret, mtx, dist, rvecs, tvecs = pickle.load(open("cal_data.pkl", "rb"))

def get_frames(cap):
    # Generate Sample Video
    frames = []
    for c in range(100):
        ret, frame = cap.read()
        if not ret:
            print("Can't receive frame (stream end?). Exiting ...")
            break
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray = cv2.undistort(gray, mtx, dist, None, mtx)
        ret, corners = get_corners(7, 7, gray)
        if ret:
            corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
            ret,rvecs, tvecs = cv2.solvePnP(objp, corners2, mtx, dist)
            imgpts, jac = cv2.projectPoints(axis, rvecs, tvecs, mtx, dist)
            frame = draw(frame , corners2, imgpts)
            frame = cv2.drawChessboardCorners(frame, (7, 7), corners2,ret)
        # Display Frame
        cv2.imshow('frame', frame)
        frames.append(frame)
        if cv2.waitKey(1) == ord('q'):
            break
    return frames


def main():
    output_file = 'sample'
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    avi_file_name = f'{output_file}.avi'

    cap = cv2.VideoCapture(0)
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







        


    