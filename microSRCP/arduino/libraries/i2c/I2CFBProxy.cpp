/*
	I2CFBProxy - Stellvertreter fuer Geraete in angeschlossenen
	I2C Board's. Fuer den I2C Master sieht es aus, als ob die
	Geraete lokal waren, alle get/set Befehle werden jedoch
	via I2C Bus gesendet bzw. empfangen.

	Dient zum Abfragen von Sensoren.

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


#include "I2CFBProxy.h"
#include "I2CDeviceManager.h"

namespace i2c
{

I2CFBProxy::I2CFBProxy( int startAddr, int endAddr, int remoteAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
	this->addr = remoteAddr;
}

int I2CFBProxy::info( int addr, srcp::feedback fb[] )
{
	uint8_t buf[4];

	buf[0] = srcp::FB;
	buf[1] = srcp::GET;
	memcpy( &buf[2], &startAddr, 2 );

	int rc = I2CDeviceManager::write( this->addr, buf, sizeof(buf) );
	if	( rc != 200 )
		return	( rc );

	rc = I2CDeviceManager::read( this->addr, buf, 1 );
	if	( rc != 200 )
		return	( rc );

	sensor = (int) buf[0];
	memset( fb, 0, 9 );

	int pos = 0;
	for	( int i = 0; i < 8; i++ )
		if	( bitRead(oldSensor, i) != bitRead(sensor,i) )
		{
			fb[pos].pin = i + this->startAddr;
			fb[pos].value = bitRead(sensor, i);
			pos++;
		}

	oldSensor = sensor;
	sensor = 0;
	return	( 200 );
}

}
