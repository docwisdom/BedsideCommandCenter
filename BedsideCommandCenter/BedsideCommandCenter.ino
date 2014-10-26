/*
BEDSIDE COMMAND CENTER (for AtMega328/Arduino Uno)

 This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License.
 http://creativecommons.org/licenses/by-nc/4.0/

 Note: This is a work in progress. The code will change, but an attempt has been made to
 provide continuity and to ensure that subsequent updates will only add functionality rather
 than remove it. However, it is recommended that you update both the ATTiny and the Atmega328
 concurrently for best performance.
*/

//*****************************************************************************
//
//   Includes
//
//*****************************************************************************

#include <SoftwareSerial.h>
#include <SmartThings.h>
#include <Wire.h>
//#include <IRremote.h>
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>

//*****************************************************************************
//
//   Define Variables
//
//*****************************************************************************

//IO def
#define pixelPIN 6
#define redPIN 9
#define greenPIN 10
#define bluePIN 11
#define motionPIN 7
#define speakerPIN 5
#define lightPIN A0
#define micPIN A1
#define sideButtonsPIN A2
#define alarmSwitchPIN A3


//smartthings IO def
#define PIN_THING_RX    3
#define PIN_THING_TX    2


// global variables
int stateSwitch; //state of switch
bool isDebugEnabled; // enable or disable debug in this example
int stateNetwork; //led displaying zigbee network state
boolean motion = false;
boolean setMotion = false;
String luxValue;

//value variables for red green and blue
int r, g, b;

//*****************************************************************************
//
//   Construct
//
//*****************************************************************************

//real time clock
RTC_DS1307 RTC;

//construct neopixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, pixelPIN, NEO_GRB + NEO_KHZ800);

//construct smartthings
SmartThingsCallout_t messageCallout;    // call out function forward decalaration
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);  // constructor

//*****************************************************************************
//
//   Setup
//
//*****************************************************************************

void setup() {
  isDebugEnabled = true; //turn on serial debug
  stateNetwork = STATE_JOINED;  // set to joined to keep state off if off

  analogReference(EXTERNAL); //external voltage reference

  pinMode(redPIN, OUTPUT); // output pin for red LED channel
  digitalWrite(redPIN, LOW); // set value to LOW (off) to match state=0
  pinMode(bluePIN, OUTPUT); // output pin for blue LED channel
  digitalWrite(bluePIN, LOW); // set value to LOW (off) to match state=0
  pinMode(greenPIN, OUTPUT); // output pin for green LED channel
  digitalWrite(greenPIN, LOW); // set value to LOW (off) to match state=0

  pinMode(pixelPIN, OUTPUT); //output pin for neopixel data
  pinMode(motionPIN, INPUT); //input pin for motion sensor

  pinMode(alarmSwitchPIN, INPUT); //input pin for alarm switch
  pinMode(sideButtonsPIN, INPUT); //input pin for side buttons

  if (isDebugEnabled)
    { // setup debug serial port
      Serial.begin(9600);         // setup serial with a baud rate of 9600
      //Serial.println("setup..");  // print out 'setup..' on start
    }

  #ifdef AVR
    Wire.begin();
  #else
    Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  #endif
    RTC.begin();

  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  strip.begin(); // Initialize comm with neopixel
  strip.show(); // Set all pixels to 'off'
  smartthing.send("src.restart");
}

//*****************************************************************************
//
//   Program Loop
//
//*****************************************************************************

void loop() {
  smartthing.run(); // run smartthing logic
  //setNetworkStateLED(); // run network check logic
  checkMotion(); // check if any of the inputs have changed
  checkLux();
  runClock();
  sendSTValues();
}

//*****************************************************************************
//
//   IO Functions
//
//*****************************************************************************

//set clock variable from RTC
void runClock() {
  if (RTC.isrunning()) { // check that real time clock is running
    DateTime now = RTC.now(); // set now variable to real time clock output
    int alarmSwitchReading = analogRead(alarmSwitchPIN);
    int sideButtonsReading = analogRead(sideButtonsPIN);
    //Serial.print("Switch:");
    //Serial.println(alarmSwitchReading);
    //Serial.print("Button:");
    //Serial.println(sideButtonsReading);
    //delay(500);
  }
}

//check motion sensor
void checkMotion() {
  setMotion = digitalRead(motionPIN);
  if (setMotion != motion){
    if (setMotion == HIGH){
      Serial.println(motion);
      smartthing.send("motion.active");
      delay(20);
      motion = setMotion;
    }
    else if (setMotion == LOW){
      smartthing.send("motion.inactive");
      Serial.println(motion);
      delay(20);
      motion = setMotion;
    }
  }
}

void checkLux() {
  float logRange = 5.0; // 3.3v = 10^5 lux
  float logLux = analogRead(lightPIN) * logRange / 1024;
  int luxInt = pow(10, logLux);
  String luxString = "lux.";
  luxValue = luxString + luxInt;
  //Serial.println(luxValue);
}

//write PWM to RGB strip
void rgb(int x, int y, int z) {
  analogWrite (redPIN, x);
  analogWrite (greenPIN, y);
  analogWrite (bluePIN, z);
  r = x;
  g = y;
  b = z;
}

//make noise on piezo
void beep1() {
  digitalWrite(speakerPIN, HIGH);
  delay(5);
  digitalWrite(speakerPIN, LOW);
  delay(110);
  digitalWrite(speakerPIN, HIGH);
  delay(5);
  digitalWrite(speakerPIN, LOW);
}

void beep2() {
  digitalWrite(speakerPIN, HIGH);
  delay(10);
  digitalWrite(speakerPIN, LOW);
  delay(110);
  digitalWrite(speakerPIN, HIGH);
  delay(1000);
  digitalWrite(speakerPIN, LOW);
}

//turn leds on
void on() {
  beep1();
  stateSwitch = 1; // save state as 1 (on)
  colorSet(strip.Color(255, 255, 255), 255); // white
  analogWrite(redPIN, 255);
  analogWrite(greenPIN, 255);
  analogWrite(bluePIN, 255);
  smartthing.send("return.on"); // send message to cloud
  Serial.println("on");
}

//turn leds off
void off() {
  beep2();
  stateSwitch = 0; // save state as 0 (off)
  colorSet(strip.Color(0, 0, 0), 0); // white
  analogWrite(redPIN, 0);
  analogWrite(greenPIN, 0);
  analogWrite(bluePIN, 0);
  smartthing.send("return.off"); // send message to cloud
  Serial.println("off");
}


//*****************************************************************************
//
//   Math Functions
//
//*****************************************************************************

//fix broken numbers
void noNegative() {
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;
}

// Fill the dots one after the other with a color
void colorSet(uint32_t c, uint8_t bright) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.setBrightness(bright);
      strip.show();
  }
}

//*****************************************************************************
//
//   Talk to SmartThings
//
//*****************************************************************************

//parses command string
void messageCallout(String message) {
  // if debug is enabled print out the received message
  if (isDebugEnabled)
  {
    Serial.print("ST: ");
    Serial.println(message);
  }

  // if message contents equals to 'on' then call on() function
  // else if message contents equals to 'off' then call off() function
  if (message.equals("on")) {
    on();
  }
  else if (message.equals("off")) {
    off();
  }
}

//send value updates every 5 minutes
void sendSTValues() {
  int lastSTSend;
  if (millis() - lastSTSend > 30000) {
    smartthing.send(luxValue);
    lastSTSend = millis();
  }
}

//*****************************************************************************
//
//   Check SmarThings Network
//
//*****************************************************************************

void setNetworkStateLED()
{
  SmartThingsNetworkState_t tempState = smartthing.shieldGetLastNetworkState();
  if (tempState != stateNetwork)
  {
    switch (tempState)
    {
      case STATE_NO_NETWORK:
        if (isDebugEnabled)
        //Serial.println("NO_NETWORK");
        smartthing.shieldSetLED(1, 0, 0); // red
        break;
      case STATE_JOINING:
        if (isDebugEnabled)
        //Serial.println("JOINING");
        smartthing.shieldSetLED(1, 0, 0); // red
        break;
      case STATE_JOINED:
        if (isDebugEnabled)
        //Serial.println("JOINED");
        smartthing.shieldSetLED(0, 1, 0); // green
        break;
      case STATE_JOINED_NOPARENT:
        if (isDebugEnabled)
        //Serial.println("JOINED_NOPARENT");
        smartthing.shieldSetLED(1, 0, 1); // purple
        break;
      case STATE_LEAVING:
        if (isDebugEnabled)
        //Serial.println("LEAVING");
        smartthing.shieldSetLED(1, 0, 0); // red
        break;
      default:
      case STATE_UNKNOWN:
        if (isDebugEnabled)
        //Serial.println("UNKNOWN");
        smartthing.shieldSetLED(0, 0, 1); // blue
        break;
    }
    stateNetwork = tempState;
  }
}

