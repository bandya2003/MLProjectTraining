# Import OpenCV module  
import cv2  
# Import pyplot from matplotlib as plt  
from matplotlib import pyplot as pltd  
# Opening the image from files  
imaging = cv2.imread("opencv-od.png")  
# Altering properties of image with cv2  
imaging_gray = cv2.cvtColor(imaging, cv2.COLOR_BGR2GRAY)  
imaging_rgb = cv2.cvtColor(imaging, cv2.COLOR_BGR2RGB)  
# Importing Haar cascade classifier xml data  
xml_data = cv2.CascadeClassifier('XML-data.xml')  
# Detecting object in the image with Haar cascade classifier   
detecting = xml_data.detectMultiScale(imaging_gray,   
                                   minSize = (30, 30))  
# Amount of object detected  
amountDetecting = len(detecting)  
# Using if condition to highlight the object detected  
if amountDetecting != 0:  
    for (a, b, width, height) in detecting:  
        cv2.rectangle(imaging_rgb, (a, b),
                      (a + height, b + width),   
                      (0, 275, 0), 9)  
# Plotting image
pltd.subplot(1, 1, 1)  
# Displaying Outut
pltd.imshow(imaging_rgb)  
pltd.show()  
pltd increment()
#plot images
