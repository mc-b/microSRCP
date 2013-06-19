/*
	microSRCPI2CSimple - einfaches I2C Slave Beispiel mit minimalen
 	Klassen. Soll die Verwendung des SRCP Protokolls ueber den I2C Bus
 	mit den LowLevel Arduino API Demonstrieren.
 
 	Fuer weitere Details siehe:
 	- https://github.com/mc-b/microSRCP/wiki/I2c-bus
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
#include <Wire.h>

#include <SRCPCommand.h>
#include <SRCPMessages.h>
#include <SRCPParser.h>
#include <Servo.h>

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration I2C
#define I2C_ADDR		1	// Eigene I2C Adresse - muss pro I2C Board angepasst werden! - Master = 0
#define I2C_OFFSET		16	// Offset, d.h. wieviele Adressen pro Board reserviert werden
#define ADDR(x)			((I2C_ADDR * I2C_OFFSET) + x)	// Berechnung effektive Adresse

// SRCP Kommando Struktur
srcp::command_t cmd;
// Servo auf Adresse 3
Servo s3;

/**
 * Daten vom I2C Bus empfangen
 */
void onReceive( int size )
{
	// CMD Struktur loeschen bzw. alle Werte welchen nicht immer gesetzt werden (z.B. Bus welche nicht uebertragen wird)
	memset( cmd.values, 0, sizeof(cmd.values) );
	cmd.bus = 0;

	// Grundinformationen, werden immer gesendet
	if	( size >= 4 )
	{
		cmd.device 	= (srcp::devices) Wire.read();
		cmd.cmd 	= (srcp::commands) Wire.read();
		char buf[2];
		Wire.readBytes( buf, 2 );
		memcpy( &cmd.addr, buf, 2 );
	}

	// Restliche Parameter abstellen, falls vorhanden
	for	( int i = 4; i < size; i++ )
		cmd.values[i-4] = Wire.read();

	switch (cmd.cmd)
	{
		case srcp::SET:
			switch (cmd.device)
			{
				case srcp::GA:
					if	( cmd.addr == ADDR(1) )
						digitalWrite( 4 + cmd.values[0], cmd.values[1] );	// Pin + Port auf Value setzen
					else if ( cmd.addr == ADDR(2) )
						digitalWrite( 6 + cmd.values[0], cmd.values[1] );
					else if ( cmd.addr == ADDR(3) )							// Servo laut Value setzen
					{
						if	( cmd.values[1] )
							s3.write( 60 );
						else
							s3.write( 120 );
					}
					break;

				case srcp::GL:
					break;

				default:
					break;
			}
			break;
		default:
			break;
	}
}

/**
 * Daten auf Anfrage zurueck an Master senden
 */
void onRequest()
{
	switch (cmd.cmd)
	{
		case srcp::GET:
			switch (cmd.device)
			{
				// muss implementiert werden, damit der Master weiss wieviele und welche Geraete hier vorhanden sind
				case srcp::DESCRIPTION:
					cmd.values[0] = cmd.values[1] = 0;			// keine Sensoren
					cmd.values[2] = ADDR(1);					// 3 x Zubehoer
					cmd.values[3] = ADDR(3);
					cmd.values[4] = cmd.values[5] = 0;			// keine Lok's
					Wire.write( (uint8_t*) cmd.args, 12 );
					return;
				default:
					break;
			}
		default:
			break;
	}
	// Default - Message o.k.
	Wire.write( 0 );
}

/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
	BEGIN( 9600 );
	INFO( "Logger ready" );

	// I2C Bus mit eigener Adresse initialisieren
	Wire.begin( I2C_ADDR );
	Wire.onReceive( onReceive );
	Wire.onRequest( onRequest );

	// Geraete initialisieren
	pinMode( 4, OUTPUT );		// Signal 1
	pinMode( 5, OUTPUT );
	pinMode( 6, OUTPUT );		// Signal 2
	pinMode( 7, OUTPUT );
	s3.attach( 3 );				// Servo an Pin 3 mit Adresse 3
}

/**
 * Endlosloop
 */
void loop()
{
	delay( 100 );
	// andere Arbeiten erledigen, z.B. Sensoren refreshen etc.
}


