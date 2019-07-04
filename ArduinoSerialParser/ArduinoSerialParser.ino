/* Serial data parser for Arduino I/O pin control, v6
   modified 7/4/2019 from source code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450
   by D. Bailey

********Incoming serial data***************

  String data received by the Arduino is parsed into four parts:
  pin-number, pin-state (1 for HIGH, 0 for LOW ), PWM value (0-255), analogRead (1 to return the value, 0 if not used)

  Examples:

  digitalWrite-
    To set digital pin 13 HIGH, send:

        (13,1,0,0)

  analogWrite-
      Use on Arduino Uno pins 3,5,6,9,10,11.
      To set PWM pin 3 to value of 150 (range is 0-255), send:

        (3,0,150,0)

  analogRead-
      To retrieve an analog value from analog A0, the string data should be in the following format:

        (0,0,0,1)
*/

//global variables

const byte numChars = 32; // max number of characters the array can hold
char receivedChars[numChars];
char tempChars[numChars];
int pin = 0; //physical I/O pin on Arduino
int pinState = 0; //1=on, 0=off
int pwmval = 0; //pwm value, range 0-255
int getval = 0; //1 = return value, 0 = do not return value
boolean newData = false;

bool analogStream = false; //set this to true to continously stream analogRead values

//attach Button.h library and create an new instance of "Button" class for each button

#include "Button.h" //this library is used to provide debounce control for pushbuttons
Button button1; //this creates an instance of "Button" class called button1.  Add more buttons as needed.


//This function reads incoming string data, and stores the characters bound by the start/end markers into an array

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte index = 0;
  char startMarker = '('; // starting marker, any characters before the first ":" are ignored
  char endMarker = ')'; //end of data string marker
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[index] = rc;
        index++;
        if (index >= numChars) {
          index = numChars - 1;
        }
      }
      else {
        receivedChars[index] = '\0'; // terminate the string
        recvInProgress = false;
        index = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


//this function tokenizes string data and parses it into three parts, then converts the number text to integers

void parseData() {    // split the string data into three parts
 
  char * strtokIndx; // this is used by strtok() as an index
  strtokIndx = strtok(tempChars, ","); // first part
  pin = atoi(strtokIndx);     // convert to a integer

  strtokIndx = strtok(NULL, ","); // second part
  pinState = atoi(strtokIndx);     // convert to a integer

  strtokIndx = strtok(NULL, ","); //third part
  pwmval = atoi(strtokIndx);     // convert to a integer

  strtokIndx = strtok(NULL, ","); //forth part
  getval = atoi(strtokIndx);     // convert to a integer
}


//this function writes and echos back digital pin-state, or PWM value to PWM pin, or returns analog value

void writePinState() {
  if (getval == 1) {
    Serial.print("analog value is: ");
    Serial.println(analogRead(pin));
  }
  else if (pwmval == 0) {
    digitalWrite(pin, pinState);
    if (pinState == 1) {
      Serial.print("pin ");
      Serial.print(pin);
      Serial.println(" is set to HIGH");
    }
    else if (pinState == 0) {
      Serial.print("pin ");
      Serial.print(pin);
      Serial.println(" is set to LOW");
    }
  }
  else {
    (analogWrite(pin, pwmval));
    Serial.print("PWM pin ");
    Serial.print(pin);
    Serial.print(" set to ");
    Serial.println(pwmval);
  }
}


//initialize serial, setup digital outputs, and setup buttons

void setup() {

  // Configure digital outputs
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT); //pwm

  // Setup button inputs
  button1.setup(2, [](int state) {  //number after the "(" refers to the pin number the button is attached to
    if (state) Serial.println("button 1 was pressed");  //put string data message between quotes;
  });

  //button2.setup(3, [](int state){  //number after the "(" refers to the pin number the button is attached to
  //  if (state) Serial.println("button 2 was pressed");  //put string data message between quotes;
  //});

  Serial.begin(9600); //set serial baud rate to 9600
  while (!Serial); //wait for serial port to open
  Serial.println("Arduino-ready...");
}

//main Loop

void loop() {
  button1.idle(); //watch for button press
  //button2.idle(); //watch for button press
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars); // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the colons with \0
    parseData();
    writePinState();
    newData = false;
  }
  //*******analog reads are sent out in constant stream if enabled*******
  if (analogStream) {
    int A0val = analogRead(A0); //read analog value from pin A0, store into variable A0val
    Serial.print("analog value is: ");
    Serial.println(A0val);
  }
}



