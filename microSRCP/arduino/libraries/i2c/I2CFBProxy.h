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

#ifndef I2CFBPROXY_H_
#define I2CFBPROXY_H_

#include <Arduino.h>
#include "../srcp/SRCPCommand.h"
#include "../srcp/SRCPFeedback.h"

namespace i2c
{

class I2CFBProxy : public srcp::SRCPFeedback
{
private:
	int startAddr;
	int endAddr;
public:
	I2CFBProxy( int startAddr, int endAddr, int remoteAddr );
	int checkAddr( int addr ) { return ( addr >= startAddr && addr <= endAddr); }
	void refresh() { /* wird vom Slave erledigt */ }
	int info( int addr, srcp::feedback fb[] );
	int	getStartAddr() { return( this->startAddr ); };
	int getEndAddr() { return( this->endAddr ); }
};

}

#endif /* I2CFBPROXY_H_ */
