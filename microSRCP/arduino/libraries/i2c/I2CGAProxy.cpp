/*
	I2CGAProxy - Stellvertreter fuer Geraete in angeschlossenen
	I2C Board's. Fuer den I2C Master sieht es aus, als ob die
	Geraete lokal waren, alle get/set Befehle werden jedoch
	via I2C Bus gesendet bzw. empfangen.

	Dient zum Schalten von Geraeten wie Servos, Lichtsignale ...

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

#include "I2CGAProxy.h"
#include "I2CDeviceManager.h"

namespace i2c
{

I2CGAProxy::I2CGAProxy( int startAddr, int endAddr, int remoteAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
	this->addr = remoteAddr;
}

int I2CGAProxy::set( int addr, int port, int value, int delay )
{
	uint8_t buf[7];
	buf[0] = srcp::GA;
	buf[1] = srcp::SET;
	memcpy( &buf[2], &addr, 2 );
	buf[4] = port;
	buf[5] = value;
	buf[6] = delay;

	return	( I2CDeviceManager::write( this->addr, buf, sizeof(buf) ) );
}

void I2CGAProxy::setPower( int on )
{
	uint8_t buf[4];
	buf[0] = srcp::POWER;
	buf[1] = srcp::SET;
	memcpy( &buf[2], &on, 2 );

	I2CDeviceManager::write( this->addr, buf, sizeof(buf) );
}

int I2CGAProxy::setSM( int bus, int addr, int device, int cv, int value )
{
	return	( I2CDeviceManager::setSM( this->addr, bus, addr, device, cv, value ) );
}

int I2CGAProxy::getSM( int bus, int addr, int device, int cv )
{
	return	( I2CDeviceManager::getSM( this->addr, bus, addr, device, cv ) );
}

}
