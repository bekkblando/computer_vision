import cv2
import numpy as np
from PIL import Image
import ffmpy
import sys
import os
import math
import time

class Face():
    
    def __init__(self, rect, gray, index):
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
        self.rect = rect
        (x,y,w,h) = rect
        self.frame = frame
        self.filtered_grey = gray[y: y + h, x: x + w]
        self.color = frame[y: y + h, x: x + w]
        self.updated = time.time()
        self.index = index
        
    def get_left_eyes(self):
        return self.left_eye_cascade.detectMultiScale(self.filtered_grey, 1.1, minNeighbors=35)
    
    def get_right_eyes(self):
        return self.right_eye_cascade.detectMultiScale(self.filtered_grey, 1.1, minNeighbors=35)
    
    def get_mouths(self):
        return self.mouth_cascade.detectMultiScale(self.filtered_grey, 1.1, minNeighbors=35, minSize=(25, 25))
    
    def get_noses(self):
        return self.nose_cascade.detectMultiScale(self.filtered_grey, 1.1, minNeighbors=35)
    
    def update(self, rect, gray):
        # Update Time
        self.rect = rect
        self.gray = gray
        self.frame = frame
        self.updated = time.time()

    def draw_cascades(self, items, frame, label):
        for (x,y,w,h) in items:
            # Change the thickness of the line to give a fade effect
            cv2.rectangle(frame,(x, y),(x + w, y + h),(0,255,0), math.floor(3 - self.decay()))
            cv2.putText(frame,f'{label}',(x + x, y + y), 1, 1, (0, 255, 0), 1)

    def decay(self):
        return time.time() - self.updated
    
    def draw(self, frame):
        self.frame = frame
        (x,y,w,h) = self.rect
        self.color = frame[y: y + h, x: x + w]
        
        # Draw Face
        self.draw_cascades([self.rect], frame, f'Face {self.index}')
        
        # Draw a rectangle around the left eyes
        self.draw_cascades(self.get_left_eyes(), self.color, "Left Eye")


        # Draw a rectangle around the right eyes
        self.draw_cascades(self.get_right_eyes(), self.color, "Right Eye")


        # Draw a rectangle around the mouth
        self.draw_cascades(self.get_mouths(), self.color, "Mouth")


        # Draw a rectangle around the nose
        self.draw_cascades(self.get_noses(), self.color, "Nose")


class Faces():
    
    def __init__(self):
        self.face_cascade = cv2.CascadeClassifier('classifiers/frontalFace10/haarcascade_frontalface_alt2.xml')
        self.faces = []
    
    
    def get_faces(self, frame):
        return self.face_cascade.detectMultiScale(frame, 1.1)
    
    
    def get_distance(self, x1, y1, x2, y2):
        return math.sqrt((x1 - x2)**2 + (y1 - y2)**2)
    
    
    def read_clean_view(self, gray, frame):
        self.same_for = 300
        self.decayed = 3
        
        # Get those rectangles around those faces
        
        faces = self.get_faces(gray)
        self.faces = list(filter(lambda face: face.decay() < self.decayed, self.faces))
                
        # Find the same faces
        for new_face in faces:
            new = True
            for face in self.faces:
                # If the same face update
                if(self.get_distance(new_face[0], new_face[1], face.rect[0], face.rect[1]) < self.same_for):
                    face.update(new_face, gray)
                    new = False
                    
            if(new):
                face = Face(new_face, gray, len(self.faces))
                self.faces.append(face)
                face.draw(frame)
        
        for face in self.faces:
            face.draw(frame)
                

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