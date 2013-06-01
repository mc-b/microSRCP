/*
	I2CGLMaster - Hilfsklasse wandelt SRCP Befehle um nach I2C.
	Dient zur Ansteuerung von Motorentreiber.

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

#include "I2CGLMaster.h"
#include "I2CUtil.h"

namespace i2c
{

I2CGLMaster::I2CGLMaster( int startAddr, int endAddr, int remoteAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
	this->addr = remoteAddr;
}

int I2CGLMaster::set( int addr, int drivemode, int v, int v_max, int fn[] )
{
	uint8_t buf[9];

	buf[0] = srcp::GL;
	buf[1] = srcp::SET;
	memcpy( &buf[2], &addr, 2 );
	buf[4] = drivemode;
	buf[5] = v;
	buf[6] = v_max;
	int f = 0;
	for	( int i = 3; i < SRCP_MAX_ARGS; i++ )
		bitWrite( f, i-3, fn[i] != 0 );
	memcpy( &buf[7], &f, 2 );

	// f0 und f1 - f12
	char a[2];
	memcpy( &a, &f, 2 );

	return	( I2CUtil::write( this->addr, buf, sizeof(buf) ) );
}

void I2CGLMaster::setPower( int on )
{
	uint8_t buf[4];
	buf[0] = srcp::POWER;
	buf[1] = srcp::SET;
	memcpy( &buf[2], &on, 2 );

	I2CUtil::write( this->addr, buf, sizeof(buf) );
}

int I2CGLMaster::setSM( int bus, int addr, int device, int cv, int value )
{
	return	( I2CUtil::setSM( this->addr, bus, addr, device, cv, value ) );
}

int I2CGLMaster::getSM( int bus, int addr, int device, int cv )
{
	return	( I2CUtil::getSM( this->addr, bus, addr, device, cv ) );
}

}
