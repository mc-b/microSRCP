/*
	I2CGAMaster - Hilfsklasse wandelt SRCP Befehle um nach I2C.
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

#ifndef I2CGAMASTER_H_
#define I2CGAMASTER_H_

#include <Arduino.h>
#include "../srcp/SRCPCommand.h"
#include "../srcp/SRCPGenericAccessoire.h"

namespace i2c
{

class I2CGAMaster :  public srcp::SRCPGenericAccessoire
{
private:
	int startAddr;
	int endAddr;
public:
	I2CGAMaster( int startAddr, int endAddr, int remoteAddr );
	int checkAddr( int addr ) { return ( addr >= startAddr && addr <= endAddr); }
	int set( int addr, int port, int value, int delay  );
	void setPower( int on );
	int setSM( int bus, int addr, int device, int cv, int value );
	int getSM( int bus, int addr, int device, int cv );
	int	getStartAddr() { return( this->startAddr ); };
	int getEndAddr() { return( this->endAddr ); }
};

}

#endif /* I2CGAMASTER_H_ */
