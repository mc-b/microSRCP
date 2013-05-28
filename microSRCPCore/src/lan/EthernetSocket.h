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

#ifndef ETHERNETSOCKET_H_
#define ETHERNETSOCKET_H_

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

#include <string.h>
#include <socket.h>
#include <w5100.h>

namespace lan
{

class EthernetSocket : public Print
{
private:
	int port;
	uint8_t sock;
public:
	EthernetSocket( int port );
	void begin();
	int available();
	int read();
	uint8_t status();
	int connected();
	void stop();
	virtual size_t write( uint8_t );
	virtual size_t write( const char *str );
	virtual size_t write( const uint8_t *buf, size_t size );
};

} /* namespace srcp */
#endif /* ETHERNETSOCKET_H_ */
