import cv2
import cv2.cv as cv
import numpy as np
import tweetpony
import time
import datetime
import sys

api = tweetpony.API(consumer_key = "", consumer_secret = "", access_token = "", access_token_secret = "")

capture = cv2.VideoCapture(0)
lastCheer = time.clock() - 3600

def tweetColor(color,frame):
    global capture
    global lastCheer
    try:
        print "========================Tweeting" + color + "==========================="
        with open("/home/root/CheerBot/stop.txt", "r+") as fo:
            fo.seek(0, 0)
            fo.write("1")
        fo.closed
        time.sleep(2)
        image_path = '/home/root/CheerBot/found.jpg'
        #fresh = cv.QueryFrame(capture)
        capture.set(3,1920)
        capture.set(4,1080)
        ret, fresh = capture.read()
        capture.set(3,320)
        capture.set(4,240)
        if not ret:
            print "Capture Failed"
        fresh = cv.fromarray(fresh)
        cv.SaveImage(image_path,fresh)
        if (3600 < (time.clock() - lastCheer)):
            salut = """@Cheerlights, """
            lastCheer = time.clock()
        else:
            salut = ''
        api.update_status_with_media(status = (salut + "I spy something "  + color + ": " + time.strftime('%m-%d-%y-%H%M', time.localtime())), media=image_path)
        with open("/home/root/CheerBot/stop.txt", "r+") as fo:
            fo.seek(0, 0)
            fo.write("0")
        fo.closed
    except tweetpony.APIError as err:
        print "Oops, something went wrong! Twitter returned error #%i and said: %s" % (err.code, err.description)

def main():
    global capture
    # captured image size
    width = 320
    height = 240

    # Over-write default captured image size
    capture.set(3,width)
    capture.set(4,height)

    #Trying to plug a memory leak
    imgHSV = cv.CreateImage((320, 240), 8, 3)
    ret, frame = capture.read()
    if not ret:
       	print "Capture Failed"
    frame = cv.fromarray(frame)
    imgHSV = cv.CreateImage(cv.GetSize(frame), 8, 3)
    cv.CvtColor(frame, imgHSV, cv.CV_BGR2HSV)
    imgProcessed = cv.CreateImage(cv.GetSize(frame), 8, 1)
    imgColorProcessed = imgProcessed
    cv.InRangeS(imgHSV, (0, 80, 80), (10, 255, 255), imgColorProcessed)
    mat = cv.GetMat(imgColorProcessed)
    moments = cv.Moments(mat, 0)
    biggest = 0
    biggestSize = 0
    redArea = pinkArea = whiteArea = GreenArea = yellowArea = BlueArea = cv.GetCentralMoment(moments, 0, 0)
    minSize = 100000
    lastTweet = time.clock()

    while True:

        #frame = cv.QueryFrame(capture)
        ret, frame = capture.read()
        if not ret:
            print "Capture Failed"
        frame = cv.fromarray(frame)
        cv.Smooth(frame, frame, cv.CV_BLUR, 3)
        biggest = 0
        minSize = 300000

        # returns thresholded image
        imgHSV = cv.CreateImage(cv.GetSize(frame), 8, 3)
        # converts BGR image to HSV
        cv.CvtColor(frame, imgHSV, cv.CV_BGR2HSV)
        imgProcessed = cv.CreateImage(cv.GetSize(frame), 8, 1)

        cv.InRangeS(imgHSV, (0, 200, 100), (3, 255, 255), imgColorProcessed)
        mat = cv.GetMat(imgColorProcessed)
        # Calculating the moments
        moments = cv.Moments(mat, 0)
        redArea = cv.GetCentralMoment(moments, 0, 0)
        if (redArea > minSize):
            #print 'Red'
            biggest = 1
            minSize = redArea

        cv.InRangeS(imgHSV, (40, 40, 40), (99, 255, 255), imgColorProcessed)
        mat = cv.GetMat(imgColorProcessed)
        # Calculating the moments
        moments = cv.Moments(mat, 0)
        greenArea = cv.GetCentralMoment(moments, 0, 0)
        if (greenArea > minSize):       	
            #print 'Green'
            biggest = 4
            minSize = greenArea
        
        cv.InRangeS(imgHSV, (20, 100, 100), (25, 255, 255), imgColorProcessed)
        mat = cv.GetMat(imgColorProcessed)
        # Calculating the moments
        moments = cv.Moments(mat, 0)
        yellowArea = cv.GetCentralMoment(moments, 0, 0)
       	if (yellowArea > minSize):	  
            #print 'Yellow'
      	    biggest = 5
            minSize = yellowArea

        cv.InRangeS(imgHSV, (100, 100, 70), (120, 255, 255), imgColorProcessed)
        mat = cv.GetMat(imgColorProcessed)
        # Calculating the moments
        moments = cv.Moments(mat, 0)
        blueArea = cv.GetCentralMoment(moments, 0, 0)
       	if (blueArea >	minSize):	  
            #print 'Blue'
            biggest = 6	
            minSize = blueArea

        if (biggest == 1):
            print 'Red Biggest at ', minSize
            if (10 < time.clock() - lastTweet):
                tweetColor("Red", frame)
                lastTweet = time.clock()
       	elif (biggest == 4):
       	    print 'Green Biggest at ', minSize
            if (10 < time.clock() - lastTweet):
                tweetColor("Green", frame)
                lastTweet = time.clock()
        elif (biggest == 5):
       	    print 'Yellow Biggest at ', minSize
            if (10 < time.clock() - lastTweet):
                tweetColor("Yellow", frame)
                lastTweet = time.clock()
       	elif (biggest == 6):
       	    print 'Blue Biggest at ', minSize
            if (10 < time.clock() - lastTweet):
                tweetColor("Blue", frame)
                lastTweet = time.clock()
        biggest = 0
    return;

if __name__ == "__main__":
    main()

