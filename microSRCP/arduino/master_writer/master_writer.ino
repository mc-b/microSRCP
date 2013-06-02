// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial3.begin( 9600 );
  Serial3.println( "I2C master ready" );
}

byte x = 0;

void loop()
{
  Wire.beginTransmission(1); // transmit to device #2
  Wire.write( 4 ); // SM
  Wire.write( 0 );  // GET
  Wire.write( 0 );  // Adresse 2 Bytes
  
  Wire.write( 0 );
  Wire.write( 1 );  // Bus
  Wire.write( 2 );  // Device
  Wire.write( x );  // CV
  Wire.endTransmission();    // stop transmitting
  
  delay( 100 );
  Wire.requestFrom(1, 1);    // request 6 bytes from slave device #2

  while(Wire.available())    // slave may send less than requested
  { 
    int c = Wire.read(); // receive a byte as character
    Serial3.print(c);         // print the character
  }
  Serial3.println();

  x++;
  delay( 100 );

}
