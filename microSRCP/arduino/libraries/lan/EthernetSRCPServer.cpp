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
#include "EthernetSRCPServer.h"

namespace lan
{
char command[64];
int pos = 0;
long last = millis();

void EthernetSRCPServer::begin( byte* mac, IPAddress ip, unsigned int port )
{
	parser = new srcp::SRCPParser();

	commandSession = new srcp::SRCPSession();
	infoSession = new srcp::SRCPSession();

	Ethernet.begin( mac, ip );
	commandSocket = new lan::EthernetSocket( port );
	infoSocket = new lan::EthernetSocket( port );
	commandSocket->begin();
	infoSocket->begin();
}

int EthernetSRCPServer::dispatch( int fbDelay )
{
	dispatch( commandSession, commandSocket, fbDelay );
	dispatch( infoSession, infoSocket, fbDelay );
	return	( 1 );
}

int EthernetSRCPServer::dispatch( srcp::SRCPSession* session, lan::EthernetSocket* socket, int fbDelay )
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
			socket->println( session->version() );
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
			parser->parse( cmd, command );
			char* rc = session->dispatch( cmd );

#if	( DEBUG_SCOPE > 0 )
			Serial3.print("send: ");
			Serial3.print( session->getStatus( ));
			Serial3.print( ", " );
			Serial3.print( rc );
			Serial3.println();
#endif
			socket->println( rc );
			pos = 0;
			break;
		}

		// Info Server
		if	( session->getStatus() == srcp::INFO && session->isPowerOn() )
		{
			// evtl. FB Module refreshen - auch wenn noch nicht gesendet wird.
			DeviceManager.refresh();

			if	( last+fbDelay < millis() )
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

} /* namespace lan */
