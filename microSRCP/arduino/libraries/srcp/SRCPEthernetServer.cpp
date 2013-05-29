/*
	SRCPEthernetServer - SRCP I/O ueber Ethernet

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

#include <Ethernet.h>
#include "SRCPEthernetServer.h"

// Debugging > 0 == ON
//#define DEBUG_SCOPE 2
// Fuer TCP ist Serial frei fuer Debugging Output
//#define Serial3 Serial

namespace srcp
{
char command[64];
int pos = 0;
long last = millis();

void SRCPEthernetServer::begin( byte* mac, IPAddress ip, unsigned int port )
{
	parser = new SRCPParser();

	commandSession = new srcp::SRCPSession();
	infoSession = new srcp::SRCPSession();

	Ethernet.begin( mac, ip );
	commandSocket = new lan::EthernetSocket( port );
	infoSocket = new lan::EthernetSocket( port );
	commandSocket->begin();
	infoSocket->begin();
}

int SRCPEthernetServer::dispatch()
{
	dispatch( commandSession, commandSocket );
	dispatch( infoSession, infoSocket );
	return	( 1 );
}

int SRCPEthernetServer::dispatch( srcp::SRCPSession* session, lan::EthernetSocket* socket )
{
	if ( socket->connected() )
	{
		// noch nicht verbunden - zuerst Versioninfo senden
		if	( session->getStatus() == srcp::UNDEFINED )
		{
#if	( DEBUG_SCOPE > 0 )
			Serial3.print( "conn : " );
			Serial3.println( session->getStatus() );
#endif
			socket->print( session->version() );
			session->setStatus( srcp::HANDSHAKE );
		}

		// verbunden - Daten empfangen
		while ( socket->available() != 0 )
		{
			int byte = socket->read();
			if ( byte != '\n' )
			{
				command[pos] = byte;
				pos++;
				continue;
			}
			if ( byte == '\r' )
				continue;
			command[pos] = '\0';

#if	( DEBUG_SCOPE > 0 )
			Serial3.print("recv: ");
			Serial3.print( session->getStatus( ));
			Serial3.print( ", " );
			Serial3.println( command );
#endif
			parser->parse( command );
			char* rc = session->dispatch();

#if	( DEBUG_SCOPE > 0 )
			Serial3.print("send: ");
			Serial3.print( session->getStatus( ));
			Serial3.print( ", " );
			Serial3.print( rc );
			Serial3.print( '\r' );
#endif
			socket->print( rc );
			pos = 0;
			break;
		}

		// Info Server
		if	( session->getStatus() == srcp::INFO && session->isPowerOn() )
		{
			// evtl. FB Module refreshen - auch wenn noch nicht gesendet wird.
			DeviceManager.refresh();

			if	( last+250 < millis() )
			{
				session->infoFeedback( socket );
				last = millis();
			}
		}
	}
	else
	{
		if	( session->getStatus() != srcp::UNDEFINED )
		{
#if	( DEBUG_SCOPE > 0 )
			Serial3.println( "disconnect" );
#endif
			session->disconnect();
			socket->stop();
			// auf ein neues!
			socket->begin();
		}
	}
}

} /* namespace srcp */
