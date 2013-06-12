/*
	microSRCPDCC - USB Board welches DCC Signale erzeugt.

	ACHTUNG: Alpha Version es gehen nur Loks bis Addr 99.

	Fuer weitere Details siehe https://github.com/mc-b/microSRCP/wiki

	Wer eine komplette DCC Zentrale sucht, sei auf http://www.fichtelbahn.de
	und http://www.opendcc.de verwiesen.

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
#include <srcp/SRCPServerSerial.h>
#include <i2c/I2CDeviceManager.h>

#include "dcc/GADCCBooster.h"
#include "dcc/GLDCCBooster.h"
#include "dccgen/DCCGenerator.h"

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
#define I2C_ADDR		0	// Eigene I2C Adresse - muss pro I2C Board angepasst werden! - Master = 0

/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
#if	( DEBUG_SCOPE > 10 )
	// Start each software serial port
	Serial3.begin( 9600 );
	Serial3.println ( "debug ready ..." );
#endif

	// DCC Initialisieren
	DCC.setup();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Definition der lokalen Geraete
	DeviceManager.addAccessoire( new dcc::GADCCBooster( 1, 2048 ) ); // Weichen, Signale Adressen 1 - 2048 abhandeln
	DeviceManager.addLoco( new dcc::GLDCCBooster( 1, 4096 ) ); // Loks mit Adressen 1 - 4096 abhandeln

#if	( DEBUG_SCOPE > 10 )
	int values[6];
	DeviceManager.getDescription( 0, 0, srcp::LAN, values );
	Serial3 << "Devices: fb " << values[0] << "-" << values[1] << ", ga " << values[2]
	       << "-" << values[3] << ", gl " << values[4] << "-" << values[5] << endl;
#endif

	// SRCP Kommunikation oeffnen
#if	( SRCP_PROTOCOL == SRCP_SERIAL )
	server.begin( 115200 );
#elif  ( SRCP_PROTOCOL == SRCP_I2C )
	// initialize I2C - Slave
	server.begin( I2C_ADDR );
#endif

#if	( DEBUG_SCOPE > 10 )
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

	// DCC Refresh
	DCC.doInstructions();
	DCC.doReadback();
}
