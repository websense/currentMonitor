/*
  Analog input, analog output, serial output

  CURRENT MONITOR
  Reads an analog input pin, 
  and convert to current 
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe 
  modified Adrian Keating and Rachel Cardell-Oliver

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial

  WARNING: for Dragino LoRa boards
  before the LoRa radio turns on there is a little hiccup to (fall to 5mA then jump up to 80mA)
  so the average base load is the 17 mA figure, not the short 5mA blips
*/

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

// Three measurement nodes (set one of these to true)
const boolean verboseflag = false; 
const boolean compressflag = false; //time series of critical points
const boolean shapeflag = true;  //triples showing shape of max current: max val, max width, low val

// working variables
int sensorValue = 0;        // value read from the pot
float CurrentNow;
float CurrentLast;
unsigned long TimeNow;
unsigned long TimeLast;
unsigned long TimeRise;

int THRESHOLD = 10; //min magnitude of change to save rising or falling edge
  
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  sensorValue = analogRead(analogInPin);
  CurrentLast = 0.1895*sensorValue+ 1.759;  // in mA
  TimeLast = micros()/1000;
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  CurrentNow = 0.1895*sensorValue + 1.759;  // in mA
  TimeNow = micros()/1000;

  if (verboseflag) { //save all measurement points
    Serial.print(TimeNow);
    Serial.print(", "); 
    Serial.println(CurrentNow);
  }
  
  //save the compressed time series (just relevant points)
  if (compressflag) { //TODO remove duplicates for falling hiccups
    if (abs(CurrentNow - CurrentLast) > THRESHOLD) {
      // print the t-1 and t results to the Serial Monitor:
       Serial.print(TimeLast);
       Serial.print(", ");   
       Serial.print(CurrentLast,1);  
       Serial.println();
       
       Serial.print(TimeNow);
       Serial.print(", ");   
       Serial.print(CurrentNow,1);
       Serial.println(); 
    }
  }

  // or save the shape: high-mA, length of high-ms,background-mA
  if (shapeflag) {
    if ((CurrentNow - CurrentLast) > THRESHOLD) { //rising edge
       TimeRise = TimeNow;
    }
    if ((CurrentLast - CurrentNow) > 2*THRESHOLD) { //falling edge but not hiccups
      // print the t-1 and t results to the Serial Monitor:
       Serial.print(CurrentLast,1); 
       Serial.print(", ");   
       Serial.print(TimeNow-TimeRise);
       Serial.print(", ");   
       Serial.print(CurrentNow,1);
       Serial.println();
    }
  }
  
  //update t-1 values for next time
  CurrentLast = CurrentNow;
  TimeLast = TimeNow;
  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
