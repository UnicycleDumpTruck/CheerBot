#!/usr/bin/python

# I put this script together to allow my Raspberry Pi to act as a color server for my Arduino Cheerlights tree
# and my big Christmas tree, which uses GE Color Effects Lights connected to the Arduino. 
# The XBee module is connected to the 3.3V, RX, and TX pins on the Pi's 
# GPIO (http://baldwisdom.com/preparing-for-a-la-mode-raspberry-pi-and-arduino/) and the console 
# was disabled (http://www.irrational.net/2012/04/19/using-the-raspberry-pis-serial-port/). 
# I have the script set to run on boot (http://elinux.org/RPi_Email_IP_On_Boot_Debian).
# The script checks the Twitter stream of my room-roaming CheerBot and also the Cheerlights service.

import serial, scrapelib, json, time, tweetpony, threading

api = tweetpony.API(consumer_key = "", consumer_secret = "", access_token = "", access_token_secret = "")

class twitterThread (threading.Thread):
    def __init__(self, threadID, name, counter):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.counter = counter
    def run(self):
        print "Starting " + self.name
        checkTwitter()
        print "Exiting " + self.name

class cheerThread (threading.Thread):
    def __init__(self, threadID, name, counter):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.counter = counter
    def run(self):
        print "Starting " + self.name
        checkCheerlights()
        print "Exiting " + self.name

def getCheerColor():
        global lastCheer
        try:
                cheerlights = json.loads(s.urlopen('http://api.thingspeak.com/channels/1417/field/1/last.json'))
#                print 'The Cheerlights color was {0} at {1}.'.format(cheerlights['field1'],time.strftime("%H:%M:%S", time.localtime()))
                cheerColor = cheerlights['field1']
#                print "Current: ", cheerColor, " Last: ", lastCheer
                if (cheerColor !=  lastCheer):
                        sendCheerColor(cheerlights['field1'])
                lastCheer = cheerColor
        except:
                print 'Error in getCheerlightsColor', sys.exc_info()[0]

def sendBotColor(color):
        try:
#                ser = serial.Serial("/dev/ttyAMA0", 9600, timeout=1)
                if ser.isOpen() == 0:
                        print 'Re-opening a serial connection'
                        ser = serial.Serial("/dev/ttyAMA0", 9600, timeout=1)
                print ser
                print "Sending " + color + "from CheerBot."
                ser.write(color)
                ser.write(",")
#                ser.close()
        except:
                print 'Error in sendBotColor'

def sendCheerColor(color):
        try:
#                ser = serial.Serial("/dev/ttyAMA0", 9600, timeout=1)
                if ser.isOpen() == 0:
                        print 'Re-opening a serial connection'
                        ser = serial.Serial("/dev/ttyAMA0", 9600, timeout=1)
                print ser
                print "Sending " + color + "from Cheerlights."
                ser.write(color)
                ser.write(",")
#                ser.close()
        except:
                print 'Error in sendCheerColor'

class StreamProcessor(tweetpony.StreamProcessor):
        def on_status(self, status):
                print "%s: %s" % (status.user.screen_name, status.text)
                tweetText = status.text
                if (tweetText.find("Blue") >= 0):
                        sendBotColor("blue")
                elif (tweetText.find("Red") >= 0):
                        sendBotColor("red")
                elif (tweetText.find("Green") >= 0):
                        sendBotColor("green")
                elif (tweetText.find("Yellow") >= 0):
                        sendBotColor("yellow")
                return True

def checkTwitter():
        processor = StreamProcessor(api)
        try:
                api.user_stream(processor = processor)
        except KeyboardInterrupt:
                pass              

def checkCheerlights():
        while True:
                try:
                        getCheerColor()
                except:
                        print 'Error in checkCheerlights'
                time.sleep(10)


ser = serial.Serial("/dev/ttyAMA0", 9600, timeout=1)
s = scrapelib.Scraper(requests_per_minute=5, follow_robots=True)
lastCheer = "nothing"

# Create new threads
thread1 = twitterThread(1, "TwitterThread", 1)
thread2 = cheerThread(2, "CheerlightsThread", 2)

# Start new Threads
thread1.start()
thread2.start()

print "Exiting Main Thread"
