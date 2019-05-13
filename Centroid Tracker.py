from collections import OrderedDict
import math

class CentroidTracker():

    def __init__(self , maxDisappeared = 5 ,  maxDistance = 80 ):
        self.nextObjectID = 0;
        self.objects = OrderedDict()
        self.disappeared = OrderedDict()
        self.maxDisappeared= maxDisappeared
        self.maxDistance  = maxDistance

        
    def register(self  , centroid ):
        self.objects[self.nextObjectID] = centroid
        self.disappeared[self.nextObjectID] = 0
        self.nextObjectID += 1
        
    def deregister(self , objectID ):
        del self.objects[objectID]
        del self.disappeared[objectID]

    def D( self , a , b , c , d ):
        return math.sqrt((a-c )*(a-c) + (b-d)*(b-d))

    def update(self , rects ) :
        if(len(rects) == 0):
            for objectID in list(self.disappeared.keys()):
                self.disappeared[objectID] += 1
                if self.disappeared[objectID] > self.maxDisappeared:
                    self.deregister(objectID)
            return self.objects

        inputCentroids = []
        for( i , (x , y , x1 , y1 )) in enumerate(rects):
             cx = ( x + x1)//2
             cy = (y + y1 )//2
             inputCentroids.append((cx , cy ))

        if( len(self.objects)==0):
             for i in  inputCentroids :
                self.register(i)
        else:
             objectCentroids = list(self.objects.values())
             idx = []
             dist = OrderedDict()
             for i , x in self.objects.items():
                 ans = (-1 , 99999999999 )
                 for j in range( 0 , len(inputCentroids)):
                     d =  self.D( x[0] , x[1], inputCentroids[j][0] , inputCentroids[j][1])
                     if( d <= ans[1]):
                           ans = ( j , d )
                 idx.append(( i , ans[0]))
                 dist[i] = int(ans[1])

             used_idx = set()
             used_current_cent= set()

             unused = set( range(0 , len(inputCentroids)))
             unused_cent = set(self.objects.keys())

             for i in range( 0 , len(idx)):
                if idx[i][0] in used_idx or idx[i][1] in used_current_cent :
                    continue

                if dist[idx[i][0]] > self.maxDistance :
                    continue

                self.objects[idx[i][0]] = inputCentroids[idx[i][1]]
                self.disappeared[idx[i][0]] = 0

                used_idx.add(idx[i][0])
                used_current_cent.add(idx[i][1])

                if idx[i][0] in unused_cent :
                    unused_cent.remove(idx[i][0])
                if idx[i][1] in unused :
                    unused.remove(idx[i][1])


           
             if(len(objectCentroids) >= len(inputCentroids)):
                 for i in unused_cent:
                     self.disappeared[i]+= 1
                     if self.disappeared[i] > self.maxDisappeared :
                         self.deregister(i)
             else:
                 for i in unused :
                     self.register(inputCentroids[i])                     
        return self.objects

                         
                     
             
             



import cv2
face_cascade = cv2.CascadeClassifier('p1.xml') 
cap = cv2.VideoCapture(0) 
ct = CentroidTracker() 
x = 0
while 1:   
    ret, img = cap.read()  
   
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) 
    faces = face_cascade.detectMultiScale(gray, 1.3, 5) 
    rect = []
    for (x,y,w,h) in faces: 
        rect.append( (x ,y , x +w , y + h) )
       
    objects = ct.update(rect)
    for (x,y,w,h) in faces: 
        cv2.rectangle(img,(x,y),(x+w,y+h),(255,255,0),2)
        
    for (objectID, centroid) in objects.items():
        text = "ID {}".format(objectID)
        cv2.putText(img, text, (centroid[0] - 10, centroid[1] - 10),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
        cv2.circle(img, (centroid[0], centroid[1]), 4, (0, 255, 0), -1)

    cv2.imshow('img',img)
    k = cv2.waitKey(1) & 0xff
    if k == 27: 
        break
cap.release() 
