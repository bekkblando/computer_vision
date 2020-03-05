import cv2
import numpy as np
from PIL import Image
import ffmpy
import sys
import os
import math

    
class Faces():
    
    def __init__(self):
        self.face_cascade = cv2.CascadeClassifier('classifiers/frontalFace10/haarcascade_frontalface_alt2.xml')
        self.left_eye_cascade = cv2.CascadeClassifier(f'{cv2.data.haarcascades}/haarcascade_lefteye_2splits.xml')
        self.right_eye_cascade = cv2.CascadeClassifier(f'{cv2.data.haarcascades}/haarcascade_righteye_2splits.xml')

        if cv2.__version__ == '4.0.0' or cv2.__version__ == '4.2.0':
            self.noseCascadeName = './classifiers/haarcascade_mcs_nose.xml'
        elif cv2.__version__ == '3.4.1':
            self.noseCascadeName = '/Nariz.xml'

        if cv2.__version__ == '4.0.0' or cv2.__version__ == '4.2.0':
            self.mouthCascadeName = '/haarcascade_smile.xml'
        elif cv2.__version__ == '3.4.1':
            self.mouthCascadeName = '/Mouth.xml'
            
        self.nose_cascade = cv2.CascadeClassifier(self.noseCascadeName)
        self.mouth_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + self.mouthCascadeName)
        
    def draw_cascades(self, items, frame, label):
        for index, (x,y,w,h) in enumerate(items):
            cv2.rectangle(frame,(x, y),(x + w, y + h),(0,255,0),2)
            cv2.putText(frame,f'{label} {index}',(x + x, y + y), 1, 1, (0, 255, 0), 1)
          
    def in_face(self, obj):
        for (x,y,w,h) in self.faces:
            if(obj[0] >= x and obj[1] >= y):
                return True
        return False
    
    def remove_not_in_face(self, objs):
        return list(filter(self.in_face, objs))
    
    def get_faces(self, frame):
        return self.face_cascade.detectMultiScale(frame, 1.1)
    
    def get_left_eyes(self, frame):
        return self.left_eye_cascade.detectMultiScale(frame, 1.1, minNeighbors=35)
    
    def get_right_eyes(self, frame):
        return self.right_eye_cascade.detectMultiScale(frame, 1.1, minNeighbors=35)
    
    def get_mouths(self, frame):
        return self.mouth_cascade.detectMultiScale(frame, 1.1, minNeighbors=35, minSize=(25, 25))
    
    def get_noses(self, frame):
        return self.nose_cascade.detectMultiScale(frame, 1.1, minNeighbors=35)
    
    
    def read_clean_view(self, gray, frame):
        self.faces = []
        self.left_eyes = []
        self.right_eyes = []
        self.mouths = []
        self.noses = []
        
        # Get those rectangles around those faces
        self.faces = self.get_faces(gray)
        self.draw_cascades(self.faces, frame, "Face")
        
        for (x, y, w, h) in self.faces:
            filtered_grey = gray[y: y + h, x: x + w]
            color = frame[y: y + h, x: x + w]

            self.draw_cascades(self.get_left_eyes(filtered_grey), color, "Left Eye")
            # Draw a rectangle around the left eyes
           

            # Draw a rectangle around the right eyes
            self.draw_cascades(self.get_right_eyes(filtered_grey), color, "Right Eye")
            

            # Draw a rectangle around the mouth
            self.draw_cascades(self.get_mouths(filtered_grey), color, "Mouth")


            # Draw a rectangle around the nose
            self.draw_cascades(self.get_noses(filtered_grey), color, "Nose")
    
     

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
    face = Faces()
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

    print("Processing Frames")
    frame_count = len(frames)
    for index, frame in enumerate(frames):
        print(f"Processed {index} out of {frame_count}\n")
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        # Draw a rectangle around the faces
        face.read_clean_view(gray, frame)
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