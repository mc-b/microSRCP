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

#include <HardwareSerial.h>
#include <Streaming.h>
#include <Wire.h>
#include "I2CDeviceManager.h"
#include "I2CFBMaster.h"
#include "I2CGAMaster.h"
#include "I2CGLMaster.h"
#include "../srcp/SRCPDeviceManager.h"

namespace i2c
{

#define CV_BOARD 		2

void I2CDeviceManager::begin()
{
	// I2C Master
	Wire.begin();

	union
	{
		uint8_t byte[12];
		int values[6];
	} buf;

#if	( DEBUG_SCOPE > 0 )
	Serial3.println( "search I2C bus" );
#endif

	// es werden max. 10 I2C Adressen unterstuetzt, sonst wird das ganze zu langsam. Weitere Boards via USB anschliessen!!!
	for	( int i = 1; i < 10; i++ )
	{
		int board = getSM( i, 0, 0, srcp::CV, CV_BOARD );
		// kein I2C Board auf dieser Adresse vorhanden?
		if	( board == -1 || board == 255 )
			continue;

		// Liefert die Adressen von/bis von FB, GA, GL
		int rc = getDescription( i, 0, 0, srcp::LAN, buf.byte );
		if	( rc == -1 )
			continue;

#if	( DEBUG_SCOPE > 0 )
			Serial3 << "I2C addr:id: " << i << ":" <<
					", fb: " << buf.values[0] << "-" << buf.values[1] <<
					", ga: " << buf.values[2] << "-" << buf.values[3] <<
					", gl: " << buf.values[4] << "-" << buf.values[5] << endl;
#endif
		// FB Geraete vorhanden
		if	( buf.values[0] > 0 && buf.values[1] > 0 )
			DeviceManager.addFeedback( new I2CFBMaster( buf.values[0], buf.values[1], i) );

		// GA Geraete vorhanden
		if	( buf.values[2] > 0 && buf.values[3] > 0 )
			DeviceManager.addAccessoire( new I2CGAMaster( buf.values[2], buf.values[3], i ) );

		// GL Geraete vorhanden
		if	( buf.values[4] > 0 && buf.values[5] > 0 )
			DeviceManager.addLoco( new I2CGLMaster( buf.values[4], buf.values[5], i ) );
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

	write	( remoteAddr, buf, sizeof(buf) );
	read	( remoteAddr, buf, 1 );
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

	write	( remoteAddr, buf, sizeof(buf) );
	return	( read	( remoteAddr, rc, 12 ) );
}

int I2CDeviceManager::write( int addr, uint8_t *buf, int size )
{
#if	( DEBUG_SCOPE > 2 )
		Serial3 << "send: " << addr;
		for	( int i = 0; i < size; i++ )
			Serial3 << ":" << buf[i];
		Serial3.println();
#endif

	Wire.beginTransmission( addr );
	Wire.write( buf, size );
	Wire.endTransmission();
	delay( 100 );

	if	( Wire.getWriteError() )		// timeout!
	{
		Wire.clearWriteError();
		return ( 417 );
	}
	return	( 200 );
}

int I2CDeviceManager::read( int addr, uint8_t *buf, int size )
{
	Wire.requestFrom( addr, size );
	memset( buf, -1, size );
	int i = 0;
	for	( ; i < size && Wire.available(); i++ )
		buf[i] = Wire.read();
	delay( 100 );

#if	( DEBUG_SCOPE > 2 )
		Serial3 << "revc: " << addr << ", " << size << ":" << i << " ";
		for	( int i = 0; i < size; i++ )
			Serial3 << ":" << buf[i];
		Serial3.println();
#endif

	return	( 200 );
}


} /* namespace i2c */
