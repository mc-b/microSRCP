
/*
	microSRCPODCC - DCC Zentrale basierend auf den Sourcen von
	http://www.opendcc.de.

	Der Code wurde ermoeglicht durch das OpenDCC Projekt und
	die Zurverfuegungstellung der Sourcen von Wolfgang Kufer.
	Besten Dank!

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

#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

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

#include <GAOpenDCC.h>
#include <GLOpenDCC.h>

// SRCP I/O Server
srcp::SRCPServerSerial server;
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

	// Port A Enable an L298N - MotoMama, DCC Signal an Input 1 + 2
	pinMode( 11, OUTPUT);
	digitalWrite( 11, HIGH );
	// Port B Enable an L298N - MotoMama, DCC Signal an Input 3 + 4
	pinMode( 12, OUTPUT);
	digitalWrite( 12, HIGH );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Definition DCC Geraete
	DeviceManager.addAccessoire( new odcc::GAOpenDCC( 1, 2048 ) ); // Weichen, Signale Adressen 1 - 2048 abhandeln
	DeviceManager.addLoco( new odcc::GLOpenDCC( 1, 4096 ) ); // Loks mit Adresessen 1 - 4096 abhandeln

	// SRCP Kommunikation oeffnen
	server.begin( 115200 );
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
