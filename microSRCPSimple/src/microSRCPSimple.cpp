/*
	microSRCPSimple - einfaches SRCP Server Beispiel mit minimalen
	Klassen. Soll die Verwendung des SRCP Protokoll mit den LowLevel
	Arduino API Demonstrieren.

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
#include <srcp/SRCPCommand.h>
#include <srcp/SRCPParser.h>
#include <srcp/SRCPMessages.h>

// SRCP Kommando Struktur
srcp::command_t cmd;
// I/O Buffer
char buf[64];
// SRCP Parser
srcp::SRCPParser parser;
// Anzahl Session bzw. GO
int counter = 0;

/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
	Serial.begin( 115200 );

#if	( DEBUG_SCOPE > 1 )
	// Start each software serial port
	Serial3.begin( 9600 );
	Serial3.println ( "debug ready ..." );
#endif
}

/**
 * Daten via Serieller Schnitstelle empfangen
 */
int receive()
{
	int count = 0;
	if ( Serial.available() )
	{
		while ( true )
		{
			if	( ! Serial.available() )
				continue;

			int i = Serial.read();
			// NL beendet lesen
			if ( i == '\n' )
				break;
			// Sonderzeichen ignorieren
			if ( i == '\r' )
				continue;
			buf[count++] = i;
		}
		buf[count] = '\0';

#if	( DEBUG_SCOPE > 1 )
		Serial3.print( "recv: " );
		Serial3.println( buf );
#endif

		// SRCP ASCII parsen und in command_t abstellen
		parser.parse( cmd, buf );
		return	( true );
	}
	return	( false );
}

/**
 * Daten verarbeiten
 */
char* dispatch()
{
	// Verzeigen laut Befehl
	switch ( cmd.cmd )
	{
		case srcp::GO:
			return( srcp::Messages.go( counter++ ) );
		case srcp::SET:
			switch ( cmd.device )
			{
				case srcp::GA:
					return( srcp::Messages.ok() );
				case srcp::GL:
					return( srcp::Messages.ok() );
				default:
					return( srcp::Messages.error(421) );
			}
			break;
		case srcp::CONNECTIONMODE:
			return( srcp::Messages.ok202() );

		case srcp::PROTOCOL:
			return( srcp::Messages.ok201() );

		default:
			return( srcp::Messages.ok() );
	}
	return	( srcp::Messages.ok() );
}


/**
 * Endlosloop
 */
void loop()
{
	if	( receive() )
	{
		char* rc = dispatch();
#if	( DEBUG_SCOPE > 1 )
		Serial3.print( "send: " );
		Serial3.println( rc );
#endif
		Serial.println( rc );
	}
	// andere Arbeiten erledigen, z.B. Sensoren refreshen etc.
}


