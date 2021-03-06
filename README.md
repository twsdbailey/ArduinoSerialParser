# ArduinoSerialParser
Control and read I/O pins on an Arduino by sending simple string commands over serial com port.

Incoming data to the Arduino is sent as a series of four comma-separated numerical characters bound by parenthesis.  
The comma-separated numerical characters are parsed into four parts: 


**pin-number** (any valid I/O pin number)

**pin-state** (1 for HIGH, 0 for LOW )

**PWM value** (0-255)

**return analogRead** (1 to return the value, 0 if not used).

Use with Serial Monitor, a terminal program such as PuTTY, or any other device capable of sending serial data


This parser used in this code is adapted from example code written by Robin2 @ http://forum.arduino.cc/index.php?topic=396450

Tested with Arduino UNO R3 @ 115200 baud, Adafruit Metro Mini @ 115200 baud, and Inland Pro-Mini @ 9600 baud
All messages sent to the Arduino from the computer will be echoed back to the computer as verification that the
message was correctly received.
 


  *Examples:*

  **Send digitalWrite command**
  
  To set digital pin 13 "HIGH", send:

        (13,1,0,0)
        
  To set digital pin 13 "LOW", send:
   
        (13,0,0,0)

  **Send analogWrite command**
  
  Use on Arduino Uno pins 3,5,6,9,10,11.
  To set PWM pin 3 to value of 150 (range is 0-255), send:

        (3,0,150,0)

  **Serial command to get analogRead value**
  
  To retrieve an analog value from analog A0, the string data should be in the following format:

        (0,0,0,1)

Multiple sets of commands can be sent at once, though doing this should done with caution. The max number of sets that can be sent at once is untested. At some point, the incoming serial buffer on the Arduino will become full, and previously sent bytes of data will end-up getting pushed out of the buffer by the incoming data before it is parsed.  Sending just a few sets at once will not be an issue.

*Example:*

 Set digital pin 13 "HIGH", and set PWM value on pwm pin 3 to "150"
 
      (13,1,0,0)(3,0,150,0)
