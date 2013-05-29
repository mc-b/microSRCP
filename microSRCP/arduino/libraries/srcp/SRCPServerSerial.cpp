/*
	SRCPServerSerial - SRCP Server welche Meldungen mittels
	der Seriellen Schnittstellen empfaengt und sendet.

	Siehe auch: http://srcpd.sourceforge.net/srcp/

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

#include "SRCPServerSerial.h"
#include <Streaming.h>
#include "SRCPMessages.h"

// Debugging > 0 == ON
#define DEBUG_SCOPE 2
// Fuer TCP ist Serial frei fuer Debugging Output
//#define Serial3 Serial

namespace srcp
{

// Input Buffer
char buf[64];
unsigned long lasts = millis();

/**
 * Oeffnet den Seriallen Port mit speed, 8 bit, No Parity und 1 Stop Bit.
 * Flow/Control unterstuetzt das Arduino Port.
 */
void SRCPServerSerial::begin(unsigned long speed)
{
#if	( DEBUG_SCOPE > 1 )
	Serial3.print( "open port 1: " );
	Serial3.println( speed );
#endif
	Serial.begin( speed );

	session = new SRCPSession();
	parser = new SRCPParser();
}

/**
 * Prueft ob Daten am Seriellen Port anliegt und wenn ja werden diese
 * Verarbeitet.
 */
int SRCPServerSerial::dispatch(void)
{
	// keine Daten vorhanden - exit
	if	( ! Serial.available() )
	{
/*		// noch nicht verbunden - zuerst Versioninfo senden
		if	( session->getStatus() == srcp::UNDEFINED )
		{
#if	( DEBUG_SCOPE > 0 )
			Serial3 << "conn : " << session->getStatus() << endl;
#endif
			Serial.print( Messages.version()  );
			delay( 1000 );
			return	( 0 );
		}*/
		// Info Server
		if	( session->getStatus() != srcp::UNDEFINED && session->isPowerOn() )
		{
			if	( lasts+250 < millis() )
			{
				session->infoFeedback( &Serial );
				Serial.flush();
				lasts = millis();
			}
		}
		return	( 0 );
	}

	int count = 0;
	while ( true )
	{
		if	( ! Serial.available() )
			continue;

		if	( session->getStatus() == srcp::UNDEFINED )
			session->setStatus( srcp::HANDSHAKE );

		int i = Serial.read();
		// NL beendet lesen
		if	( i == '\n' )
		{
			if	( count == 0 )
				continue;
			break;
		}
		// Sonderzeichen ignorieren
		if	( i == '\r' )
			continue;
		buf[count++] = i;
	}
	buf[count] = '\0';

#if	( DEBUG_SCOPE > 0 )
	Serial3.print("recv: ");
	Serial3.print( session->getStatus( ));
	Serial3.print( ", " );
	Serial3.println( buf );
#endif

	// ASCII SRCP Commands parsen und abstellen in global_cmd
	parser->parse( buf );
	// SRCP Commands verarbeiten, in rc steht die SRCP Rueckmeldung
	char* rc = session->dispatch();

#if	( DEBUG_SCOPE > 0 )
	Serial3.print("send: ");
	Serial3.print( session->getStatus( ));
	Serial3.print( ", " );
	Serial3.print( rc );
	Serial3.print( '\r' );
#endif

	// Rueckmeldung an Host
	Serial.write( rc );
	Serial.flush();

	return	( 1 );
}

} /* namespace srcp */
