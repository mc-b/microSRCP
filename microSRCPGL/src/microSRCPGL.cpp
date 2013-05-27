/*
	microGL - USB Board um 2 analoge Stromkreise zu regeln.

	Bestehend aus:
	- Arduino Board mit min. ATmega328P - http://www.arduino.cc
	- Ethernet Shield
	- Motor Shield - http://arduino.cc/en/Main/ArduinoMotorShield oder
	http://www.nkcelectronics.com/freeduino-arduino-motor-control-shield-kit.html

	Die Kommunikation mit dem Host (z.B. PC mit RocRail) findet mittels dem
	SRCP (http://http://srcpd.sourceforge.net/srcp/) Protokoll statt.

	Copyright (c) 2013 Marcel Bernet.  All right reserved.

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

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Streaming.h>
#include <srcp/SRCPCommand.h>
#include <srcp/SRCPDeviceManager.h>
#include <dev/GASignal.h>
#include <dev/GAServo.h>
#include <dev/GLMotoMamaAnalog.h>
#include <dev/FBSwitchSensor.h>

// Globaler Command Buffer
srcp::command_t global_cmd;

void setup()
{
#if	( DEBUG_SCOPE > 1 )
	// Start each software serial port
	Serial3.begin( 9600 );
	Serial3 << "debug ready ..." << endl;
#endif
	// SRCP Kommunikation oeffnen
	//SRCPServer.begin( mac, ip, localPort );
	//Serial << "Server listen " << ip << "." << localPort << endl;

	// Geraete initialisieren, je nach Board und Verwendung
	DeviceManager.addAccessoire( new dev::GASignal( 1, 4, 5 ) ); // Signal mit Addr 1 und 2 Led an Pin 4 und 5
	DeviceManager.addAccessoire( new dev::GASignal( 2, 6, 7 ) );
	DeviceManager.addAccessoire( new dev::GAServo( 3, 2, 60, 90 ) ); // Servo mit Addr 3 an Pin 2, min. Stellung 60, max. Stellung 90 von 180.
	DeviceManager.addAccessoire( new dev::GAServo( 4, 3, 60, 90 ) );
	DeviceManager.addFeedback( new dev::FBSwitchSensor( 1, A0, A5 ) ); // im Moment wird nur 1 FBSwitchSensor mit max. 8 Meldern unterstuetzt!
#if ( __AVR_ATmega1280__ || __AVR_ATmega2560__ )
	//DeviceManager.addDevice( new dev::GLMotoMamaAnalog( 1, 10,  8,  9 ) ); // Moto Mama Shield, Pin 10 Geschwindigkeit, 8 Vor-, 9 Rueckwaerts - nur Mega
#endif
	//DeviceManager.addDevice( new dev::GLMotoMamaAnalog( 2, 11, 12, 13 ) );
	Serial << "Devices ready" << endl;
}

void loop()
{
	DeviceManager.setGA( 1, 0, 1, 0 );
	DeviceManager.setGA( 2, 0, 0, 0 );
	delay( 1000 );
	DeviceManager.setGA( 1, 0, 0, 0 );
	DeviceManager.setGA( 2, 0, 1, 0 );
	delay( 1000 );


}




