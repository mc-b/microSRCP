// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void requestEvent()
{

  Serial3.println( "send" );
  Wire.write( 200 ); // respond with message of 6 bytes
                       // as expected by master
                
                        
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  Serial3.print( "recv " );
  Serial3.println( howMany );
  
  for  ( int i = 0; i < howMany; i++ )
  {
    char c = Wire.read(); // receive byte as a character
    Serial3.print( (char) (c + '0') );         // print the character
    Serial3.print( ", " );
  }
  Serial3.println();         // print the integer
}

void setup()
{
  Serial3.begin(9600);           // start serial for output
  Serial3.println( "I2C Slave ready" );
  
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  Wire.begin(1);                // join i2c bus with address #4    
}

void loop()
{
}


