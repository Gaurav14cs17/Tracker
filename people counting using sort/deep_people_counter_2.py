
from pyimagesearch.centroidtracker import CentroidTracker
from pyimagesearch.trackableobject import TrackableObject
from imutils.video import VideoStream
from imutils.video import FPS
import numpy as np
import argparse
import imutils
import time
import dlib
import cv2

import sort

my_tracker = sort.Sort()
memory = {}
COLORS = np.random.randint(0, 255, size=(200, 3),dtype="uint8")




CLASSES = ["background", "aeroplane", "bicycle", "bird", "boat",
	"bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
	"dog", "horse", "motorbike", "person", "pottedplant", "sheep",
	"sofa", "train", "tvmonitor"]

# load our serialized model from disk
print("[INFO] loading model...")
net = cv2.dnn.readNetFromCaffe("C:/Users/Itrs/Downloads/people-counting-opencv/mobilenet_ssd/MobileNetSSD_deploy.prototxt" ,"C:/Users/Itrs/Downloads/people-counting-opencv/mobilenet_ssd/MobileNetSSD_deploy.caffemodel")
print("[INFO] opening video file...")
Svs = cv2.VideoCapture("C:/Users/Itrs/Downloads/people-counting-opencv/videos/example_01.mp4")

# initialize the video writer (we'll instantiate later if need be)
writer = None
W = None
H = None
ct = CentroidTracker(maxDisappeared=10, maxDistance=80)
trackers = []
trackableObjects = {}
totalFrames = 0
totalDown = 0
totalUp = 0
tracks_value = []
fps = FPS().start()

def post_process(img):
    object_detected = dict()
    (H, W) = frame.shape[:2]
    blob = cv2.dnn.blobFromImage(frame, 0.007843, (W, H), 127.5)
    net.setInput(blob)
    detections = net.forward()
    for i in np.arange(0, detections.shape[2]):
        confidence = detections[0, 0, i, 2]
        if confidence > 0.6:
            classId = int(detections[0, 0, i, 1])
            box = detections[0, 0, i, 3:7] * np.array([W, H, W, H])
            (startX, startY, endX, endY) = box.astype("int")
            label = CLASSES[classId]
            if label != "person":
                continue
            i = 0
            label_with_num  = str(label) + '_' + str(i)
            while True:
                if label_with_num not in object_detected.keys():
                    break
                label_with_num = str(label) + '_' + str(i)
                i  = i +1
                
            object_detected[label_with_num] = [(int(startX),int(startY),int(endX - startX ), int(endY - startY )),confidence]
    return object_detected      
    
def intermediate_detections(img):
    objects_detected  = post_process(img)
    objects_list = list(objects_detected.keys())
    trackers_dict = dict() 
    if len(objects_list) > 0:
        trackers_dict = {key : cv2.TrackerKCF_create() for key in objects_list}
        for item in objects_list:
            trackers_dict[item].init(frame, objects_detected[item][0])
            
    return objects_detected, objects_list, trackers_dict
    
    

# loop over frames from the video stream
frame = Svs.read()
frame = frame[1]
objects_detected, objects_list, trackers_dict = intermediate_detections(frame)    
while True:
    frame = Svs.read()
    frame = frame[1]
    print(frame.shape)
    frame = cv2.resize(frame , (512 , 512))
    rgb =  cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    print(frame.shape)
    dets =  []
    if W is None or H is None:
        (H, W) = frame.shape[:2]
        
    
    if len(objects_detected) > 0:
        del_items = []
        for obj,tracker in trackers_dict.items():
            ok, bbox = tracker.update(frame)
            if ok:
                objects_detected[obj][0] = bbox
            else:
                del_items.append(obj) 
        
        for item in del_items:
            trackers_dict.pop(item)
            objects_detected.pop(item)
            
    if len(objects_detected) > 0:
        for key , box in objects_detected.items():
            print(",,,,,,,,,," ,box)
            dets.append([box[0][0], box[0][1], box[0][0]+ box[0][2], box[0][1] + box[0][3] , box[1] ])
            
           
    else:
        objects_detected, objects_list, trackers_dict = intermediate_detections(frame)   
            
            
        
    '''
    dets =  []
    if totalFrames % 7 == 0:
     
        trackers = []
        blob = cv2.dnn.blobFromImage(frame, 0.007843, (W, H), 127.5)
        net.setInput(blob)
        detections = net.forward()
        for i in np.arange(0, detections.shape[2]):
            confidence = detections[0, 0, i, 2]
            if confidence > 0.6:
                idx = int(detections[0, 0, i, 1])
                if CLASSES[idx] != "person" :
                    continue
                box = detections[0, 0, i, 3:7] * np.array([W, H, W, H])
                (startX, startY, endX, endY) = box.astype("int")
                tracker = cv2.TrackerCSRT_create()
                ok = tracker.init(rgb ,(startX, startY, endX-startX, endY-startY))
                #   dets.append([x, y, x+w, y+h, confidences[i]])
                dets.append([int(startX), int(startY), int(endX), int(endY) ,0.6])
                trackers.append(tracker)
    else:
        for tracker in trackers:
            ok , pos = tracker.update(rgb)
            startX = int(pos[0])
            startY = int(pos[1])
            endX =   int(pos[2])
            endY =   int(pos[3])
            dets.append([int(startX), int(startY), int( startX + endX), int(startY + endY) ,0.6])
        
     '''       
   
    if len(dets):
        tracks_value = my_tracker.update(np.asarray(dets))
        
    for track in tracks_value:
        (x, y) = (int(track[0]), int(track[1]))
        (w, h) = (int(track[2]), int(track[3]))
        color = [int(c) for c in COLORS[ int(track[4]) % len(COLORS)]]
        cv2.rectangle(frame, (x, y), (w, h), color, 3)
        text = "{}".format(int(track[4]))
        cv2.putText(frame, text, (x, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)
            
    print("FPS ", fps)
    cv2.imshow("Frame", frame)
    key = cv2.waitKey(1) & 0xFF
    if key == ord("q"):
        break
    totalFrames += 1
    fps.update()

# stop the timer and display FPS information
fps.stop()
print("[INFO] elapsed time: {:.2f}".format(fps.elapsed()))
print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))

# check to see if we need to release the video writer pointer
if writer is not None:
	writer.release()



# close any open windows
cv2.destroyAllWindows()