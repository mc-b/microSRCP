/*
	microSRCPSimple - einfaches SRCP Server Beispiel mit minimalen
 	Klassen. Soll die Verwendung des SRCP Protokoll mit den LowLevel
 	Arduino API Demonstrieren.
 
 	Fuer weitere Details siehe:
 	- https://github.com/mc-b/microSRCP/wiki/Srcp-protokoll 
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
#include <SoftwareSerial.h>
#include <Logger.h>			// in dieser Datei kann das Logging an/abgeschaltet werden

#include <SPI.h>
#include <Ethernet.h>

#include <SRCPCommand.h>
#include <SRCPMessages.h>
#include <SRCPParser.h>
#include <Servo.h>

// SRCP Kommando Struktur
srcp::command_t cmd;
// I/O Buffer
char buf[64];
// SRCP Parser
srcp::SRCPParser parser;
// Anzahl Session bzw. GO
int counter = 0;
// Servo auf Adresse 3
Servo s3;

/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
	Serial.begin( 115200 );

	BEGIN( 9600 );
	INFO( "Logger ready" );

	// Geraete initialisieren
	pinMode( 4, OUTPUT );		// Signal 1
	pinMode( 5, OUTPUT );
	pinMode( 6, OUTPUT );		// Signal 2
	pinMode( 7, OUTPUT );
	s3.attach( 3 );				// Servo an Pin 3 mit Adresse 3
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

		DEBUG2( "recv", buf )

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
			return( srcp::Messages.go( ++counter ) );
		case srcp::SET:
			switch ( cmd.device )
			{
				case srcp::GA:
					if	( cmd.addr == 1 )
						digitalWrite( 4 + cmd.values[0], cmd.values[1] );	// Pin + Port auf Value setzen
					else if ( cmd.addr == 2 )
						digitalWrite( 6 + cmd.values[0], cmd.values[1] );
					else if ( cmd.addr == 3 )								// Servo laut Value setzen
					{
						if	( cmd.values[1] )
							s3.write( 60 );
						else
							s3.write( 120 );
					}
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
		DEBUG2( "send", rc );
		Serial.println( rc );
	}
	// andere Arbeiten erledigen, z.B. Sensoren refreshen etc.
}