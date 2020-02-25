import cv2
import numpy as np
from PIL import Image
import ffmpy
import sys
import os


def get_frames(vid):
    frames = []
    counter = 0
    while vid.isOpened():
        ret, frame = vid.read()
        if not ret:
            print("Stream End - Exiting \n")
            break
        # cv2.imshow('frame',frame)
        frames.append(frame)
    return frames


def main():
    input = sys.argv[1]
    output_file = sys.argv[2]
    vid = cv2.VideoCapture(input)
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    avi_file_name = f'{output_file}.avi'

    # Get the height and width
    w = vid.get(cv2.CAP_PROP_FRAME_WIDTH)
    h = vid.get(cv2.CAP_PROP_FRAME_HEIGHT)

    out = cv2.VideoWriter(avi_file_name, fourcc, 20.0, (int(w),int(h)))
    frames = get_frames(vid)

    for frame in frames:
        out.write(frame)
    out.release()
    vid.release()
    cv2.destroyAllWindows()

    output_file_name = f'{output_file}.mp4'
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