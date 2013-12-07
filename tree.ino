#include <SPI.h>
#include <SoftwareSerial.h>

SoftwareSerial xBeeSerial =  SoftwareSerial(8, 9);
String response;

//#define xmas_color_t uint16_t // typedefs can cause trouble in the Arduino environment  
#define XMAS_LIGHT_COUNT          (25) //I only have a 36 light strand. Should be 50 or 36  
#define XMAS_CHANNEL_MAX          (0xF)  
#define XMAS_DEFAULT_INTENSITY     (0xCC)  
#define XMAS_HUE_MAX               ((XMAS_CHANNEL_MAX+1)*6-1)  
#define XMAS_COLOR(r,g,b)     ((r)+((g)<<4)+((b)<<8))  
#define XMAS_COLOR_WHITE     XMAS_COLOR(XMAS_CHANNEL_MAX,XMAS_CHANNEL_MAX,XMAS_CHANNEL_MAX)  
#define XMAS_COLOR_BLACK     XMAS_COLOR(0,0,0)  
#define XMAS_COLOR_RED          XMAS_COLOR(XMAS_CHANNEL_MAX,0,0)  
#define XMAS_COLOR_GREEN     XMAS_COLOR(0,XMAS_CHANNEL_MAX,0)  
#define XMAS_COLOR_BLUE          XMAS_COLOR(0,0,XMAS_CHANNEL_MAX)  
#define XMAS_COLOR_CYAN          XMAS_COLOR(0,XMAS_CHANNEL_MAX,XMAS_CHANNEL_MAX)  
#define XMAS_COLOR_MAGENTA     XMAS_COLOR(XMAS_CHANNEL_MAX,0,XMAS_CHANNEL_MAX)  
#define XMAS_COLOR_YELLOW     XMAS_COLOR(XMAS_CHANNEL_MAX,XMAS_CHANNEL_MAX,0)  
   
// Pin setup  
#define XMASPIN 7 // I drive the LED strand from pins 4 and 7
#define XMASPINTWO 4
#define STATUSPIN 13 // The LED  

int ledAnalogOne[] = {3, 5, 6}; 
//the three pins of the first analog LED 3 = redPin, 5 = greenPin, 6 = bluePin                                
//These pins must be PWM

//Defined Colors (different RGB (red, green, blue) values for colors
const byte RED[] = {0, 255, 255}; 
const byte ORANGE[] = {172, 251, 255}; 
const byte YELLOW[] = {0, 200, 255}; 
const byte GREEN[] = {255, 0, 255}; 
const byte BLUE[] = {255, 255, 0}; 
const byte CYAN[] = {255, 0, 0}; 
const byte MAGENTA[] = {0, 255, 200}; 
const byte WHITE[] = {0, 50, 150}; 
const byte WARMWHITE[] = {0, 170, 240};
const byte BLACK[] = {255, 255, 255}; //?
const byte PURPLE[] = {100, 255, 160}; 

static uint16_t c;
String lastCommandString = "";
byte lastCommand[] = {255, 255, 255};

   
// The delays in the begin, one, and zero functions look funny, but they give the correct  
// pulse durations when checked with a logic analyzer. Tested on an Arduino Uno.  
   
 void xmas_begin(int pin)  
 {  
  digitalWrite(pin,1);  
  delayMicroseconds(7); //The pulse should be 10 uS long, but I had to hand tune the delays. They work for me  
  digitalWrite(pin,0);   
 }  
   
 void xmas_one(int pin)  
 {  
  digitalWrite(pin,0);  
  delayMicroseconds(11); //This results in a 20 uS long low  
  digitalWrite(pin,1);  
  delayMicroseconds(7);   
  digitalWrite(pin,0);  
 }  
   
 void xmas_zero(int pin)  
 {  
  digitalWrite(pin,0);  
  delayMicroseconds(2);   
  digitalWrite(pin,1);  
  delayMicroseconds(20-3);   
  digitalWrite(pin,0);  
 }  
   
 void xmas_end(int pin)  
 {  
  digitalWrite(pin,0);  
  delayMicroseconds(40); // Can be made shorter  
 }  
   
   
 // The rest of Robert's code is basically unchanged  
   
 void xmas_fill_color(uint8_t begin,uint8_t count,uint8_t intensity,uint16_t color)  
 {  
      uint8_t led = count;
      while(led--)  
      {  
           xmas_set_color(XMASPIN,begin++,intensity,color);
           delay(100);
      }
      delay(100);
      led = count;
      while(led--)  
      {  
           xmas_set_color(XMASPINTWO,begin++,intensity,color);
           delay(100);
      }      
 }  
   
 void xmas_fill_color_same(uint8_t begin,uint8_t count,uint8_t intensity,uint16_t color)  
 {  
      while(count--)  
      {  
           xmas_set_color(XMASPIN,0,intensity,color);  
      }  
 }  
   
   
 void xmas_set_color(uint8_t pin,uint8_t led,uint8_t intensity,uint16_t color) {  
      uint8_t i;  

      xmas_begin(pin);  
      for(i=6;i;i--,(led<<=1))  
           if(led&(1<<5))  
                xmas_one(pin);  
           else  
                xmas_zero(pin);  
      for(i=8;i;i--,(intensity<<=1))  
           if(intensity&(1<<7))  
                xmas_one(pin);  
           else  
                xmas_zero(pin);  
      for(i=12;i;i--,(color<<=1))  
           if(color&(1<<11))  
                xmas_one(pin);  
           else  
                xmas_zero(pin);  
      xmas_end(pin);  
}  
   
   
uint16_t xmas_color(uint8_t r,uint8_t g,uint8_t b) {  
      return XMAS_COLOR(r,g,b);  
 }  
   
uint16_t xmas_color_hue(uint8_t h) {  
      switch(h>>4) {  
           case 0:     h-=0; return xmas_color(h,XMAS_CHANNEL_MAX,0);  
           case 1:     h-=16; return xmas_color(XMAS_CHANNEL_MAX,(XMAS_CHANNEL_MAX-h),0);  
           case 2:     h-=32; return xmas_color(XMAS_CHANNEL_MAX,0,h);  
           case 3:     h-=48; return xmas_color((XMAS_CHANNEL_MAX-h),0,XMAS_CHANNEL_MAX);  
           case 4:     h-=64; return xmas_color(0,h,XMAS_CHANNEL_MAX);  
           case 5:     h-=80; return xmas_color(0,XMAS_CHANNEL_MAX,(XMAS_CHANNEL_MAX-h));  
      }  
 }  


void setup() {
  pinMode(XMASPIN, OUTPUT);  
  pinMode(XMASPINTWO, OUTPUT);
  pinMode(STATUSPIN, OUTPUT);  
  xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_BLACK); //Enumerate all the lights  
  //xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_BLUE); //Make them all blue  

  for(int i = 0; i < 3; i++) {   
    pinMode(ledAnalogOne[i], OUTPUT);   //Set the three LED pins as outputs   
  }  
  setColor(ledAnalogOne, BLACK);       //Turn off led 1  
  xBeeSerial.begin(9600);
  xBeeSerial.println("Ardino booted.");
  Serial.begin(9600);
  delay(100); 
  Serial.flush();
  delay(100);
  Serial.println("Setup completed.");
}

void loop()
{
  updateColor();
  delay(5000);
}

void updateColor()                     
{
  Serial.println("updateColor was called.");
  char charIn;
//  xBeeSerial.println("CheerlightsColor");
//  delay(5000);
  while(xBeeSerial.available()){  //is there anything to read?
    charIn = xBeeSerial.read(); // read a char from the buffer
    if (charIn == ',') {
      Serial.println(response);
      // Light the tree
      if (response=="warmwhite")
      {  
         fadeToColor(ledAnalogOne, lastCommand, WARMWHITE, 10);
         //delay(3000);
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = WARMWHITE[i];
         }
         lastCommandString = "whitewhite";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_WHITE);
      }
      else if (response=="black")
      {  
         fadeToColor(ledAnalogOne, lastCommand, BLACK, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = BLACK[i];
         }  
         lastCommandString = "black";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_BLACK);
      }
      else if (response=="red")
      {  
         fadeToColor(ledAnalogOne, lastCommand, RED, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = RED[i];
         }  
         lastCommandString = "red";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_RED);
      }
      else if (response=="green")
      {  
         fadeToColor(ledAnalogOne, lastCommand, GREEN, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = GREEN[i];
         }  
         lastCommandString = "green";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_GREEN);
      }
      else if (response=="blue")
      {  
         fadeToColor(ledAnalogOne, lastCommand, BLUE, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = BLUE[i];
         }  
         lastCommandString = "blue";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_BLUE);
      }
      else if (response=="cyan")
      {  
         fadeToColor(ledAnalogOne, lastCommand, CYAN, 10); 
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = CYAN[i];
         }  
         lastCommandString = "cyan";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_CYAN);
      }
      else if (response=="magenta")
      {  
         fadeToColor(ledAnalogOne, lastCommand, MAGENTA, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = MAGENTA[i];
         }  
         lastCommandString = "magenta";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_MAGENTA);
      }
      else if (response=="yellow")
      {  
         fadeToColor(ledAnalogOne, lastCommand, YELLOW, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = YELLOW[i];
         }  
         lastCommandString = "yellow";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_YELLOW);
      }
      else if (response=="purple")
      {  
         fadeToColor(ledAnalogOne, lastCommand, PURPLE, 10);  
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = PURPLE[i];
         }  
         lastCommandString = "purple";
         //xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_PURPLE);
      }
      else if (response=="orange")
      {  
         fadeToColor(ledAnalogOne, lastCommand, ORANGE, 10); 
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = ORANGE[i];
         }  
         lastCommandString = "orange";  
         //xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_ORANGE);
      }
      else if (response=="white")
      {  
         fadeToColor(ledAnalogOne, lastCommand, WHITE, 10);
         for(int i = 0; i < 3; i++) {   
            lastCommand[i] = WHITE[i];
         }  
         lastCommandString = "white";
         xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_WHITE);
      }
      else if (response=="black")
      {  
        fadeToColor(ledAnalogOne, lastCommand, BLACK, 10);
        for(int i = 0; i < 3; i++) {   
            lastCommand[i] = BLACK[i];
        }  
        lastCommandString = "black";
        xmas_fill_color(0,XMAS_LIGHT_COUNT,XMAS_DEFAULT_INTENSITY,XMAS_COLOR_BLACK);
      }
      else
      {
        Serial.println("No match.");
        //lastCommand = "(no match)";  
      }
      response = "";
      xBeeSerial.println(lastCommandString);
      Serial.println("End of color processing loop");
    } else {
      response += charIn; // append that char to the string response
      Serial.print(charIn);
      Serial.println(" added");
      Serial.print("Response: ");
      Serial.println(response);
    }  
  }

    
  
} // End loop


/* Sets the color of the LED to any RGB Value   led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin))   color - (byte array of three values defing an RGB color to display (color[0] = new Red value, color[1] = new Green value, color[2] = new Red value*/
void setTheColor(int* led, byte* color){ 
  
  for(int i = 0; i < 3; i++){             //iterate through each of the three pins (red green blue)   
    analogWrite(led[i], 255 - color[i]);  //set the analog output value of each pin to the input value (ie led[0] (red pin) to 255- color[0] (red input color)                                         
    //we use 255 - the value because our RGB LED is common anode, this means a color is full on when we output analogWrite(pin, 0)                                         
    //and off when we output analogWrite(pin, 255).  
    //Serial.print("Setting:");
    //Serial.println(i);
  }
}

/* A version of setColor that takes a predefined color (neccesary to allow const int pre-defined colors */
void setColor(int* led, const byte* color) { 
  byte tempByte[] = {color[0], color[1], color[2]}; 
  setTheColor(led, tempByte);
}

/* Fades the LED from a start color to an end color at fadeSpeed   
led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin))   
startCcolor - (byte array of three values defing the start RGB color (startColor[0] = start Red value, startColor[1] = start Green value, startColor[2] = start Red value   
endCcolor - (byte array of three values defing the finished RGB color (endColor[0] = end Red value, endColor[1] = end Green value, endColor[2] = end Red value   
fadeSpeed - this is the delay in milliseconds between steps, defines the speed of the fade*/ 
void fadeToNumColor(int* led, byte* startColor, byte* endColor, int fadeSpeed) {  
  int changeRed = endColor[0] - startColor[0];                             //the difference in the two colors for the red channel  
  int changeGreen = endColor[1] - startColor[1];                           //the difference in the two colors for the green channel   
  int changeBlue = endColor[2] - startColor[2];                            //the difference in the two colors for the blue channel  
  int steps = max(abs(changeRed),max(abs(changeGreen), abs(changeBlue)));  //make the number of change steps the maximum channel change    
  for(int i = 0 ; i < steps; i++) {                                        //iterate for the channel with the maximum change   
    byte newRed = startColor[0] + (i * changeRed / steps);                 //the newRed intensity dependant on the start intensity and the change determined above   
    byte newGreen = startColor[1] + (i * changeGreen / steps);             //the newGreen intensity   
    byte newBlue = startColor[2] + (i * changeBlue / steps);               //the newBlue intensity   
    byte newColor[] = {newRed, newGreen, newBlue};                         //Define an RGB color array for the new color   
    setColor(led, newColor);                                               //Set the LED to the calculated value   
    delay(fadeSpeed);                                                      //Delay fadeSpeed milliseconds before going on to the next color  
  }
  setColor(led, endColor);                                                 //The LED should be at the endColor but set to endColor to avoid rounding errors
}

/* A version of fadeToColor that takes predefined colors (neccesary to allow const int pre-defined colors */
void fadeToColor(int* led, const byte* startColor, const byte* endColor, int fadeSpeed) {  
  byte tempByte1[] = {startColor[0], startColor[1], startColor[2]};   
  byte tempByte2[] = {endColor[0], endColor[1], endColor[2]};   
  fadeToNumColor(led, tempByte1, tempByte2, fadeSpeed);
}
