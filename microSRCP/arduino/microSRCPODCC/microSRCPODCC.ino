
/*
	microSRCPODCC - DCC Zentrale basierend auf den Sourcen von
	http://www.opendcc.de.

	Der Code wurde ermoeglicht durch das OpenDCC Projekt und
	die Zurverfuegungstellung der Sourcen von Wolfgang Kufer.
	Besten Dank!

	Fuer benoetigte Shields und Details siehe: 
	- https://github.com/mc-b/microSRCP/wiki/Steuerungdigital
	- https://github.com/mc-b/microSRCP/wiki

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

extern "C"
{
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>

#include "config.h"                // general structures and definitions - make your changes here
#include "parser.h"
#include "dccout.h"                // make dcc
#include "organizer.h"             // manage commands
}

#include <SRCPCommand.h>
#include <SRCPDevice.h>
#include <SRCPDeviceManager.h>
#include <SRCPFeedback.h>
#include <SRCPGenericAccessoire.h>
#include <SRCPGenericLoco.h>
#include <SRCPMessages.h>
#include <SRCPParser.h>
#include <SRCPServerSerial.h>
#include <SRCPSession.h>

#include <Wire.h>
#include <I2CDeviceManager.h>
#include <I2CFBProxy.h>
#include <I2CGAProxy.h>
#include <I2CGLProxy.h>
#include <I2CServer.h>

#include <GAOpenDCC.h>
#include <GLOpenDCC.h>

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration Protokoll
#define SRCP_SERIAL		101
#define SRCP_I2C		102
#define SRCP_PROTOCOL	SRCP_SERIAL

#if	( SRCP_PROTOCOL == SRCP_SERIAL )
// SRCP I/O Server
srcp::SRCPServerSerial server;
#elif  ( SRCP_PROTOCOL == SRCP_I2C )
// SRCP I2C - Slave
i2c::I2CServer server = WireServer;
#else
#error "kein Prokotoll definiert"
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration I2C
#define I2C_ADDR		7	// Eigene I2C Adresse - muss pro I2C Board angepasst werden! - Master = 0

// DCC Status
t_opendcc_state opendcc_state; 

/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
	////////////////////////////////////////////////////////////////////////////////////
	// DCC Initialisieren
	init_main();
	init_dccout(); // timing engine for dcc

	init_organizer(); // engine for command repetition, memory of loco speeds and types
	opendcc_state = RUN_OKAY;

#if (__AVR_ATmega328P__)	// Arduino UNO - DCC Signal an Pin 9 + 10, Enable 11 + 12
	// Port A Enable an L298N - MotoMama, DCC Signal an Input 1 + 2
	pinMode( 11, OUTPUT);
	digitalWrite( 11, HIGH );
	// Port B Enable an L298N - MotoMama, DCC Signal an Input 3 + 4
	pinMode( 12, OUTPUT);
	digitalWrite( 12, HIGH );
#elif (__AVR_ATmega1280__ || __AVR_ATmega2560__) // Arduino Mega - DCC Signal an Pin 11 + 12, Enable 9 + 10
	// Port A Enable an L298N - MotoMama, DCC Signal an Input 1 + 2
	pinMode( 9, OUTPUT);
	digitalWrite( 9, HIGH );
	// Port B Enable an L298N - MotoMama, DCC Signal an Input 3 + 4
	pinMode( 10, OUTPUT);
	digitalWrite( 10, HIGH );	
#endif
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Definition DCC Geraete
	DeviceManager.addAccessoire( new odcc::GAOpenDCC( 1, 2048 ) ); // Weichen, Signale Adressen 1 - 2048 abhandeln
	DeviceManager.addLoco( new odcc::GLOpenDCC( 1, 4096 ) ); // Loks mit Adresessen 1 - 4096 abhandeln

	// SRCP Kommunikation oeffnen
#if	( SRCP_PROTOCOL == SRCP_SERIAL )
	server.begin( 115200 );
#elif  ( SRCP_PROTOCOL == SRCP_I2C )
	// initialize I2C - Slave
	server.begin( I2C_ADDR );
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

	run_organizer(); // run command organizer, depending on state,
	delay( 10 );
}
