
/*
	microGL - USB Board um 2 analoge Stromkreise zu regeln.

	Bestehend aus:
	- Arduino Board mit min. ATmega328P - http://www.arduino.cc
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

// Debugging > 0 == ON
#define DEBUG_SCOPE 0

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Streaming.h>

#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

#include <SRCPCommand.h>
#include <SRCPDevice.h>
#include <SRCPDeviceManager.h>
#include <SRCPEthernetServer.h>
#include <SRCPFeedback.h>
#include <SRCPGenericAccessoire.h>
#include <SRCPGenericLoco.h>
#include <SRCPMessages.h>
#include <SRCPParser.h>
#include <SRCPServerSerial.h>
#include <SRCPSession.h>

#include <EthernetSocket.h>

#include <Wire.h>
#include <I2CDeviceManager.h>
#include <I2CFBMaster.h>
#include <I2CGAMaster.h>
#include <I2CGLMaster.h>
#include <I2CServer.h>

#include <Servo.h>
#include <FBSwitchSensor.h>
#include <GALed.h>
#include <GAPWMServo.h>
#include <GAServo.h>
#include <GASignal.h>
#include <GLAnalog293.h>
#include <GLArduinoMotor.h>
#include <GLMotoMamaAnalog.h>

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration Protokoll
#define SRCP_ETHERNET	100
#define SRCP_SERIAL		101
#define SRCP_I2C		102
#define SRCP_PROTOCOL	SRCP_I2C

#if	( SRCP_PROTOCOL == SRCP_SERIAL )
// SRCP I/O Server
srcp::SRCPServerSerial server;
#elif ( SRCP_PROTOCOL == SRCP_ETHERNET )
srcp::SRCPEthernetServer server;
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip( 192, 168, 178, 241 );
#elif  ( SRCP_PROTOCOL == SRCP_I2C )
// SRCP I2C - Slave
i2c::I2CServer server = WireServer;
#else
#error "kein Prokotoll definiert"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration Board
#define BOARD_STANDARD		200
#define BOARD_I2C_MASTER 	201

#define BOARD 	BOARD_STANDARD

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration I2C
#define I2C_ADDR		1
#define I2C_ENABLED		1

/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
#if	( DEBUG_SCOPE > 1 )
	// Start each software serial port
	Serial3.begin( 9600 );
	Serial3.println ( "debug ready ..." );
#endif

#if	( BOARD == BOARD_STANDARD )
	// Geraete initialisieren, je nach Board und Verwendung
	DeviceManager.addAccessoire( new dev::GALed( 1, 4, LOW ) ); 	// 2 Signale mit 2 LED an Ports 4 - 7.
	DeviceManager.addAccessoire( new dev::GALed( 2, 5, HIGH ) );
	DeviceManager.addAccessoire( new dev::GALed( 3, 6, LOW ) );
	DeviceManager.addAccessoire( new dev::GALed( 4, 7, HIGH ) );
	DeviceManager.addAccessoire( new dev::GAServo( 3, 2, 60, 90 ) ); // Servo mit Addr 3 an Pin 2, min. Stellung 60, max. Stellung 90 von 180.
	DeviceManager.addAccessoire( new dev::GAServo( 4, 3, 60, 90 ) );
	DeviceManager.addFeedback( new dev::FBSwitchSensor( 1, A0, A3 ) ); // Sensoren, jeweils in Gruppen von 8 (auch wenn nicht 8 Pins belegt). A4+A5 = I2C Bus
#if ( __AVR_ATmega1280__ || __AVR_ATmega2560__ )
	DeviceManager.addLoco( new dev::GLMotoMamaAnalog( 1, 10,  8,  9 ) ); // Moto Mama Shield, Pin 10 Geschwindigkeit, 8 Vor-, 9 Rueckwaerts - nur Mega
#endif
	DeviceManager.addLoco( new dev::GLMotoMamaAnalog( 2, 11, 12, 13 ) );
#endif

#if	( SRCP_PROTOCOL != SRCP_I2C && I2C_ENABLED )
	// initialize I2C - Master braucht keine Adresse
	i2c::I2CDeviceManager::begin( 9, 9, 8, 8 );		// weitere Boards am I2C Bus, beginnend mit Adresse 9. Jedes Board bekommt 8 Adressen
#endif

#if	( DEBUG_SCOPE > 1 )
	Serial3.println ( "Devices ready" );
#endif

	// SRCP Kommunikation oeffnen
#if	( SRCP_PROTOCOL == SRCP_SERIAL )
	server.begin( 115200 );
#elif ( SRCP_PROTOCOL == SRCP_ETHERNET )
	server.begin( mac, ip, 4303 );
#elif  ( SRCP_PROTOCOL == SRCP_I2C )
	// initialize I2C - Slave
	WireServer.begin( I2C_ADDR );
#endif

#if	( DEBUG_SCOPE > 1 )
	Serial3.println ( "Server listen " );
#endif
}

/**
 * Endlosloop
 */
void loop()
{
	// Host Meldungen verarbeiten
	server.dispatch();

	// Refresh der Sensoren bzw. Abfragen ob Aenderungen stattgefunden haben
	DeviceManager.refresh();
}
