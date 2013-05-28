/*
	EthernetSocket - Ersatz fuer die Klassen Server und Client
	aus der Arduino Library.

	Diese konnten nicht verwendet werden, weil:
	- keine Abfrage auf connected wenn keine Daten vorhanden sind
	- keine zwei listen Ports auf dem gleichen Port.

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

#include "EthernetSocket.h"

namespace lan
{
EthernetSocket::EthernetSocket( int port )
{
	this->port = port;
	this->sock = -1;

	for ( int i = 0; i < MAX_SOCK_NUM; i++ )
	{
		if	( Ethernet._server_port[i] == 0 )
		{
			Ethernet._server_port[i] = this->port;
			this->sock = i;
			break;
		}
	}
}

void EthernetSocket::begin()
{
	socket( sock, SnMR::TCP, port, 0 );
	listen( sock );
}

uint8_t EthernetSocket::status()
{
	return W5100.readSnSR( sock );
}

int EthernetSocket::connected()
{
	return ( status() == SnSR::ESTABLISHED );
}

int EthernetSocket::available()
{
	return W5100.getRXReceivedSize( sock );
}

int EthernetSocket::read()
{
	uint8_t b;
	if ( !available() )
		return -1;
	recv( sock, &b, 1 );
	return b;
}

void EthernetSocket::stop()
{
	// attempt to close the connection gracefully (send a FIN to other side)
	disconnect( sock );
	unsigned long start = millis();

	// wait a second for the connection to close
	while ( status() != SnSR::CLOSED && millis() - start < 1000 )
		delay( 1 );

	// if it hasn't closed, close it forcefully
	if ( status() != SnSR::CLOSED )
		close( sock );
}

size_t EthernetSocket::write( uint8_t b )
{
	if ( sock != -1 )
		return	( send( sock, &b, 1 ) );
	return	( 0 );
}

size_t EthernetSocket::write( const char *str )
{
	if ( sock != -1 )
		return	( send( sock, (const uint8_t *) str, strlen( str ) ) );
	return	( 0 );
}

size_t EthernetSocket::write( const uint8_t *buf, size_t size )
{
	if ( sock != -1 )
		return	( send( sock, buf, size ) );
	return	( 0 );
}

} /* namespace srcp */
