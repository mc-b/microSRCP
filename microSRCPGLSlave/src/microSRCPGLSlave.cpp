/*
	microSRCPGL - Arduino Standardboard fungiert als Slave zur
	Zentrale und wir mittels I2C Bus an diese angeschlossen.

	Fuer weitere Details siehe https://github.com/mc-b/microSRCP/wiki

	Copyright (c) 2010 - 2013 Marcel Bernet.  All right reserved.

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
#include <srcp/SRCPServerSerial.h>
#include <srcp/SRCPEthernetServer.h>
#include <i2c/I2CDeviceManager.h>
#include <i2c/I2CServer.h>
#include <dev/GASignal.h>
#include <dev/GAServo.h>
#include <dev/GLMotoMamaAnalog.h>
#include <dev/FBSwitchSensor.h>

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
#define I2C_ADDR		1	// Eigene I2C Adresse - muss pro I2C Board angepasst werden!
#define I2C_OFFSET		16	// Offset, d.h. wieviele Adressen pro Board reserviert werden
#define I2C_ENABLED		1
#define ADDR(x)			((I2C_ADDR * I2C_OFFSET) + x)	// Berechnung effektive Adresse
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
	DeviceManager.addAccessoire( new dev::GASignal( ADDR(1), 4, 5 ) ); 			// 2 Signale mit 2 LED an Ports 4 - 7.
	DeviceManager.addAccessoire( new dev::GASignal( ADDR(2), 6, 7 ) );
	DeviceManager.addAccessoire( new dev::GAServo( ADDR(3), 2, 60, 90 ) ); 		// Servo mit Addr 3 an Pin 2, min. Stellung 60, max. Stellung 90 von 180.
	DeviceManager.addAccessoire( new dev::GAServo( ADDR(4), 3, 60, 90 ) );
	DeviceManager.addFeedback( new dev::FBSwitchSensor( ADDR(1), A0, A3 ) ); 	// Sensoren, jeweils in Gruppen von 8 (auch wenn nicht 8 Pins belegt). A4+A5 = I2C Bus
#if ( __AVR_ATmega1280__ || __AVR_ATmega2560__ )
	DeviceManager.addFeedback( new dev::FBSwitchSensor( ADDR(9), A8, A15 ) ); 	// Sensoren, Mega 8 zusaetzlich
	DeviceManager.addLoco( new dev::GLMotoMamaAnalog( ADDR(1), 10,  8,  9 ) ); 	// Moto Mama Shield, Pin 10 Geschwindigkeit, 8 Vor-, 9 Rueckwaerts - nur Mega
#endif
	DeviceManager.addLoco( new dev::GLMotoMamaAnalog( ADDR(2), 11, 12, 13 ) );
#endif

#if	( SRCP_PROTOCOL != SRCP_I2C && I2C_ENABLED )
	// initialize I2C - Master braucht keine Adresse
	i2c::I2CDeviceManager::begin();		// weitere Boards am I2C Bus, beginnend mit Adressen (I2C_ADDR * I2C_OFFSET) + x).
#endif

#if	( DEBUG_SCOPE > 1 )
	int values[6];
	DeviceManager.getDescription( 0, 0, srcp::LAN, values );
	Serial3 << "Devices: fb " << values[0] << "-" << values[1] << ", ga " << values[2]
	       << "-" << values[3] << ", gl " << values[4] << "-" << values[5] << endl;
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
