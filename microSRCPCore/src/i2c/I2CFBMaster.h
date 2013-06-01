/*
	I2CFBMaster - Hilfsklasse wandelt SRCP Befehle um nach I2C.
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

#ifndef I2CFBMASTER_H_
#define I2CFBMASTER_H_

#include <Arduino.h>
#include "../srcp/SRCPCommand.h"
#include "../srcp/SRCPFeedback.h"

namespace i2c
{

class I2CFBMaster : public srcp::SRCPFeedback
{
private:
	int startAddr;
	int endAddr;
public:
	I2CFBMaster( int startAddr, int endAddr, int remoteAddr );
	int checkAddr( int addr ) { return ( addr >= startAddr && addr <= endAddr); }
	void refresh() { /* wird vom Slave erledigt */ }
	int info( int addr, srcp::feedback fb[] );
	int	getStartAddr() { return( this->startAddr ); };
	int getEndAddr() { return( this->endAddr ); }
};

}

#endif /* I2CFBMASTER_H_ */
