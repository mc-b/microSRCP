/*
  Example for different sending methods
  
  http://code.google.com/p/rc-switch/
  
  Need help? http://forum.ardumote.com
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {

  Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);
  
  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
  
}

void loop() {

  /* Dip 123 ON, 45 OFF, Schalter A on, B,C,D,E off */
  mySwitch.switchOn("11100", "10000");
  delay(5000);
  mySwitch.switchOff("11100", "10000");
  delay(5000);

  /* Same switch as above, but using decimal code */
//  mySwitch.send(83281, 24);
//  delay(3000);  
//  mySwitch.send(83284, 24);
//  delay(3000);  
}
