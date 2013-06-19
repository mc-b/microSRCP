/*
	I2CDeviceManager - sucht den I2C Bus ab und erstellt lokale
	Proxies fuer die Remote Geraete. Damit wird eine moeglichst
	effiziente I2C Abhandlung gewaehrleistet.

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

#include "../log/Logger.h"
#include <Wire.h>
#include "I2CDeviceManager.h"
#include "I2CFBProxy.h"
#include "I2CGAProxy.h"
#include "I2CGLProxy.h"
#include "../srcp/SRCPDeviceManager.h"

namespace i2c
{

void I2CDeviceManager::begin( int devices )
{
	// I2C Master
	Wire.begin();

	union
	{
		uint8_t byte[12];
		int values[6];
	} buf;

	INFO( "search I2C bus" );


	// es werden max. 10 I2C Adressen unterstuetzt, sonst wird das ganze zu langsam. Weitere Boards via USB anschliessen!!!
	for	( int i = 1; i < devices; i++ )
	{
		int board = getSM( i, 0, 0, srcp::CV, 0 );
		// kein I2C Board auf dieser Adresse vorhanden?
		if	( board == -1 || board == 255 )
			continue;

		// Liefert die Adressen von/bis von FB, GA, GL
		int rc = getDescription( i, 0, 0, srcp::LAN, buf.byte );
		if	( rc == -1 )
			continue;

#if ( LOGGER_LEVEL >= INFO_LEVEL )
			INFO( "I2C addr");
			Logger.print ( i );
			Logger.print( ", FB ");
			Logger.print( buf.values[0] );
			Logger.print( "-" );
			Logger.print( buf.values[1] );
			Logger.print( ", GA ");
			Logger.print( buf.values[2] );
			Logger.print( "-" );
			Logger.print( buf.values[3] );
			Logger.print( ", GL ");
			Logger.print( buf.values[4] );
			Logger.print( "-" );
			Logger.print( buf.values[5] );
			Logger.println();
#endif
		// FB Geraete vorhanden
		if	( buf.values[0] > 0 && buf.values[1] > 0 )
		{
			// pro 8 Sensoren ein Feedbackmodul erstellen
			for	( int s = buf.values[0]; s < buf.values[1]; s += 8 )
				DeviceManager.addFeedback( new I2CFBProxy( s, s+8, i) );
		}

		// GA Geraete vorhanden
		if	( buf.values[2] > 0 && buf.values[3] > 0 )
			DeviceManager.addAccessoire( new I2CGAProxy( buf.values[2], buf.values[3], i ) );

		// GL Geraete vorhanden
		if	( buf.values[4] > 0 && buf.values[5] > 0 )
			DeviceManager.addLoco( new I2CGLProxy( buf.values[4], buf.values[5], i ) );
	}
}

int I2CDeviceManager::setSM( int remoteAddr, int bus, int addr, int device, int cv, int value )
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

	return	( write( remoteAddr, buf, sizeof(buf) ) );
}

int I2CDeviceManager::getSM( int remoteAddr, int bus, int addr, int device, int cv )
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

	write	( remoteAddr, buf, sizeof(buf), 100 );
	read	( remoteAddr, buf, 1, 100 );
	return	( buf[0] );
}

int I2CDeviceManager::getDescription( int remoteAddr, int bus, int addr, int device, uint8_t* rc )
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

	write	( remoteAddr, buf, sizeof(buf), 100 );
	return	( read	( remoteAddr, rc, 12, 100 ) );
}

int I2CDeviceManager::write( int addr, uint8_t *buf, int size, int wait )
{
#if ( LOGGER_LEVEL >= TRACE_LEVEL )
		TRACE ( "send" );
		Logger.print( addr );
		for	( int i = 0; i < size; i++ )
		{
			Logger.print( ":" );
			Logger.print( buf[i] );
		}
		Logger.println();
#endif

	Wire.beginTransmission( addr );
	Wire.write( buf, size );
	Wire.endTransmission();
	delay( wait );

	if	( Wire.getWriteError() )		// timeout!
	{
		Wire.clearWriteError();
		return ( 417 );
	}
	return	( 200 );
}

int I2CDeviceManager::read( int addr, uint8_t *buf, int size, int wait )
{
	Wire.requestFrom( addr, size );
	memset( buf, -1, size );
	int i = 0;
	for	( ; i < size && Wire.available(); i++ )
		buf[i] = Wire.read();
	delay( wait );

#if ( LOGGER_LEVEL >= TRACE_LEVEL )
		TRACE( "revc" );
		Logger.print( addr );
		Logger.print( ", " );
		Logger.print( size );
		Logger.print( ":" );
		Logger.print( i );
		for	( int i = 0; i < size; i++ )
		{
			Logger.print( ":" );
			Logger.print( buf[i] );
		}
		Logger.println();

#endif

	return	( 200 );
}


} /* namespace i2c */
