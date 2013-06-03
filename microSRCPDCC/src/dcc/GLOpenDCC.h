/*
	GLOpenDCC - Steuerung von Lokomotiven
	anhand des DCC Signals.

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

#ifndef GLOPENDCC_H_
#define GLOPENDCC_H_

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <srcp/SRCPGenericLoco.h>

namespace dcc
{

class GLOpenDCC : public srcp::SRCPGenericLoco
{
private:
	int startAddr;
	int endAddr;
public:
	GLOpenDCC( int startAddr, int endAddr );
	int set( int addr, int drivemode, int v, int v_max, int fn[] );
	int checkAddr( int addr ) { return ( addr >= startAddr && addr <= endAddr); }
	void setPower( int on );
	int	getStartAddr() { return( this->startAddr ); };
	int getEndAddr() { return( this->endAddr ); }
	int setSM( int bus, int addr, int device, int cv, int value );
	int getSM( int bus, int addr, int device, int cv );

};

}

#endif /* GLOPENDCC_H_ */
