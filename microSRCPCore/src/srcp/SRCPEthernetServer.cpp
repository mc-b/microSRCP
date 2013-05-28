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

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Streaming.h>
#include <Ethernet.h>
#include "SRCPEthernetServer.h"

namespace srcp
{
char command[64];
int pos = 0;
long lasts = millis();

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
			Serial << "conn : " << session->getStatus() << endl;
#endif
			socket->print( session->version() );
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
			Serial << "data : " << session->getStatus() << ", " << command << endl;
#endif
			parser->parse( command );
			char* rc = session->dispatch();

#if	( DEBUG_SCOPE > 0 )
			Serial << "rc   : " << session->getStatus() << ", " << rc << '\r';
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

			if	( lasts+250 < millis() )
			{
				session->infoFeedback( socket );
				lasts = millis();
			}
		}
	}
	else
	{
		if	( session->getStatus() != srcp::UNDEFINED )
		{
#if	( DEBUG_SCOPE > 0 )
			Serial << "disconnect" << endl;
#endif
			session->disconnect();
			socket->stop();
			// auf ein neues!
			socket->begin();
		}
	}
}

} /* namespace srcp */
