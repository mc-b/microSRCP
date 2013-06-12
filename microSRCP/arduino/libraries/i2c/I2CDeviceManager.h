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
#ifndef I2CDEVICEMANAGER_H_
#define I2CDEVICEMANAGER_H_

namespace i2c
{

class I2CDeviceManager
{
public:
	static void begin();
	static int setSM( int remoteAddr, int bus, int addr, int device, int cv, int value );
	static int getSM( int remoteAddr, int bus, int addr, int device, int cv );
	static int getDescription( int remoteAddr, int bus, int addr, int device, uint8_t* rc );
	static int read( int addr, uint8_t *buf, int size, int wait = 10 );
	static int write( int addr, uint8_t *buf, int size, int wait = 5 );

};

} /* namespace i2c */
#endif /* I2CDEVICEMANAGER_H_ */
