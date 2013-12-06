import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.PWM as PWM
import time
import random

random.seed()

centerThreshold = 100
rightThreshold = 200
leftThreshold = 200
left = 0
right = 0
center = 0

try:
    ADC.setup()
    PWM.start("P8_13", 95.0, 50)
    PWM.start("P9_14", 95.0, 50)

    while(True):
        with open("/home/root/CheerBot/stop.txt", "r") as fo:
            fo.seek(0, 0)
            status = fo.read(1)
        fo.closed
       	if (status == "1"): # Stopping for a picture
            print "============================================ Stopping for a picture ======================"
            PWM.set_duty_cycle("P8_13", 0.0)
            PWM.set_duty_cycle("P9_14", 0.0)
            time.sleep(5)
            print "Going again"
        ADC.read_raw("P9_38")
        ADC.read_raw("P9_38")
        left = ADC.read_raw("P9_38")
#        print "Left: ", left
        ADC.read_raw("P9_39")
        ADC.read_raw("P9_39")
        center = ADC.read_raw("P9_39")
#        print "Center: ", center
        ADC.read_raw("P9_40")
        ADC.read_raw("P9_40")
        right = ADC.read_raw("P9_40")
#        print "Right: ", right

        
        if (center >= centerThreshold):
            if (left >= leftThreshold and right >= rightThreshold):
                if (random.randint(0,1)):
                    PWM.set_duty_cycle("P8_13", 10.0) 
                    PWM.set_duty_cycle("P9_14", 10.0) #Backwards
                    time.sleep(.5)
#                    print "All detect, turning Left"
                else:
                    PWM.set_duty_cycle("P8_13", 4.0) #Backwards
                    PWM.set_duty_cycle("P9_14", 4.0) 
                    time.sleep(.5)
#                    print "All detect, turning Right"
            elif (left >= leftThreshold):
                PWM.set_duty_cycle("P8_13", 4.0) #Backwards                 
                PWM.set_duty_cycle("P9_14", 4.0)
#                print "Center and Left detect, turning Right"
            elif (right >= rightThreshold):
                PWM.set_duty_cycle("P8_13", 10.0)
       	        PWM.set_duty_cycle("P9_14", 10.0) #Backwards
#       	       	print "Center and Right detect, turning Left"
            else:
                PWM.set_duty_cycle("P8_13", 0.0)
                PWM.set_duty_cycle("P9_14", 4.0)
#                print "Center Detect, turning Right"

        elif (left >= leftThreshold):
            PWM.set_duty_cycle("P8_13", 0.0)
            PWM.set_duty_cycle("P9_14", 4.0)
#            print "Left Detect, Turning Right"
        elif (right >= rightThreshold):
            PWM.set_duty_cycle("P8_13", 10.0)      
            PWM.set_duty_cycle("P9_14", 0.0)
#            print "Right Detect, Turning Left"
        else:
            PWM.set_duty_cycle("P8_13", 10.0)
            PWM.set_duty_cycle("P9_14", 4.0)
#            print "Forward"

except:
    PWM.stop("P8_13")
    PWM.stop("P9_14")
    PWM.cleanup()
    print "Interrupt detected, stopped PWM."
#    raise
