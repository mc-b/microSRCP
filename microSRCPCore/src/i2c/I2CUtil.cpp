/*
	I2CUtil - Hilfsklasse fuer den I2C Bus.
	Stellt LowLevel Funktionen wie write/read zur Verfuegung.
	Auf die Arduino Wire Library kann dadurch komplett verzichtet
	werden (spart Speicherplatz) und die Kommunikation ist
	sicherer durch Timeout und wiederholtes Senden bei Fehler.

	Copyright (c) 2010 Marcel Bernet.  All right reserved.

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
#include <Streaming.h>
#include "twi.h"
#include <string.h>
#include "I2CUtil.h"
#include "../srcp/SRCPCommand.h"


namespace i2c
{

void I2CUtil::begin( int addr = 0 )
{
	twi_init();
	if ( addr != 0 )
		twi_setAddress( addr );
}

int I2CUtil::write( int addr, uint8_t *buf, int size )
{
#if	( DEBUG_SCOPE > 2 )
		Serial3 << "send to: " << addr << ", args " << (int) buf[0] << ":"  << (int) buf[1]
				<< ", size " << size << endl;
#endif
	// mehrmals versuchen ob Kommunikation klappt - evtl. muss auf den Bus gewartet werden.
	for ( int i = 0; i < 3; i++ )
	{
		int rc = twi_writeTo( addr, buf, size, 1, true );
		if ( rc == 0 )
			return (200);

		delay( 5 );
	}
	// timeout!
	return (417);
}

int I2CUtil::read( int addr, uint8_t *buf, int size )
{
	// mehrmals versuchen ob Kommunikation klappt - evtl. muss auf den Bus gewartet werden.
	for ( int i = 0; i < 5; i++ )
	{
		int rc = twi_readFrom( addr, buf, size, true );
		if ( rc == size )
			return (200);

		delay( 10 );
	}
	// timeout!
	return (417);
}

int I2CUtil::setSM( int remoteAddr, int bus, int addr, int device, int cv, int value )
{
	uint8_t buf[8];
	buf[0] = srcp::SM;
	buf[1] = srcp::SET;
	int a = 0;
	// nicht Board Eeprom aendern?
	if	( bus != 0 )
		a = addr;
	memcpy( &buf[2], &a, 2 );
	buf[4] = bus;
	buf[5] = device;
	buf[6] = cv;
	buf[7] = value;

	return	( I2CUtil::write( remoteAddr, buf, sizeof(buf) ) );
}

int I2CUtil::getSM( int remoteAddr, int bus, int addr, int device, int cv )
{
	uint8_t buf[7];
	buf[0] = srcp::SM;
	buf[1] = srcp::GET;
	int a = 0;
	// nicht Board Eeprom aendern?
	if	( bus != 0 )
		a = addr;
	memcpy( &buf[2], &a, 2 );
	buf[4] = bus;
	buf[5] = device;
	buf[6] = cv;

	int rc = I2CUtil::write( remoteAddr, buf, sizeof(buf) );
	if	( rc != 200 )
		return	( -1 );

	rc = I2CUtil::read( remoteAddr, buf, 1 );
	if	( rc != 200 )
		return	( -1 );

	return	( buf[0] );
}

int I2CUtil::getDescription( int remoteAddr, int bus, int addr, int device, uint8_t* rc )
{
	uint8_t buf[6];
	buf[0] = srcp::DESCRIPTION;
	buf[1] = srcp::GET;
	int a = 0;
	// nicht Board Eeprom aendern?
	if	( bus != 0 )
		a = addr;
	memcpy( &buf[2], &a, 2 );
	buf[4] = bus;
	buf[5] = device;

	int r = I2CUtil::write( remoteAddr, buf, sizeof(buf) );
	if	( r != 200 )
		return	( -1 );

	r = I2CUtil::read( remoteAddr, rc, 12 );
	if	( r != 200 )
		return	( -1 );

	return	( 200 );
}

}
