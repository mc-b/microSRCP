/*
	microSRCPV2 - Sketch zum Testen der microSRCPV2 Platine.
	
	Implementiert einen Pendelzug mit zwei Abstellgleisen.
	
	Beschreibung siehe: https://github.com/mc-b/microSRCP/wiki/GrundkomponentenV2

	Copyright (c) 2010 - 2014 Marcel Bernet.  All right reserved.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <Servo.h>

Servo d2;

//////////////////////////////////////////////////////////////////////////////////////////
// Setup alle benoetigen Sensoren und Aktoren
void setup() 
{                
  // Taster
  pinMode(A0, INPUT_PULLUP );
  pinMode(A1, INPUT_PULLUP );
  pinMode(A2, INPUT_PULLUP );  

  // Led's exkl. Led 5 welche an A3 Poti haengt
  pinMode(4, OUTPUT);     
  pinMode(6, OUTPUT);     
  pinMode(7, OUTPUT);    

  // Servo - Weiche 
  d2.attach( 2 ); 

  // L293D - Motordriver
  pinMode(8, OUTPUT);     
  pinMode(9, OUTPUT);  

}

//////////////////////////////////////////////////////////////////////////////////////////
// Hauptprogramm
void loop() 
{
  int sensorValue = analogRead(A3);
  analogWrite( 5, sensorValue / 4 );  
  
    // vorwaerts fahren
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite( 3, sensorValue / 4 );
  digitalWrite( 6, LOW );
  digitalWrite( 7, HIGH );
  d2.write( 60 );
  
  while  ( digitalRead( A1 ) )
    delay( 10 );
  
  analogWrite(3, 0); // Stop 
  delay(1000); // 1 Sekunden Wartezeit
  
  // rueckwaerts fahren
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  analogWrite( 3, sensorValue / 4 );
  digitalWrite( 6, HIGH );
  digitalWrite( 7, LOW );
  
  while  ( digitalRead( A0 ) )
    delay( 10 );  
  
  analogWrite(3, 0); // Stop 
  delay(1000); // 1 Sekunden Wartezeit

    // vorwaerts fahren
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite( 3, sensorValue / 4 );
  digitalWrite( 6, LOW );
  digitalWrite( 7, HIGH );
  d2.write( 95 );  
  
  while  ( digitalRead( A2 ) )
    delay( 10 );
  
  analogWrite(3, 0); // Stop 
  delay(1000); // 1 Sekunden Wartezeit
  
  // rueckwaerts fahren
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  analogWrite( 3, sensorValue / 4 );
  digitalWrite( 6, HIGH );
  digitalWrite( 7, LOW );
  
  while  ( digitalRead( A0 ) )
    delay( 10 );  
 
  analogWrite(3, 0); // Stop 
  delay(1000); // 1 Sekunden Wartezeit
}



