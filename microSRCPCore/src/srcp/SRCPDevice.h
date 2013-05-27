/*
	SRCPElement - Hauptklasse fuer alle Arten von SRCP Geraeten.

	Siehe auch: http://srcpd.sourceforge.net/srcp/

	Copyright (c) 2012 Marcel Bernet.  All right reserved.

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

#ifndef SRCPDEVICE_H_
#define SRCPDEVICE_H_

#include "SRCPCommand.h"

namespace srcp
{

class SRCPDevice
{
protected:
	int addr;
public:
	virtual int checkAddr( int addr ) { return( this->addr == addr ); }
	virtual int	getStartAddr() { return( this->addr ); };
	virtual int getEndAddr() { return( this->addr ); }
	virtual int getAddr() { return( this->addr ); }

	// Setzen von Geraete Eigenschaften
	virtual uint8_t get( int addr ) { return( -1 ); }

	// evtl. Refresh, weiterschalten o.ae. Wird aufgerufen wenn keine Kommandos anstehen
	virtual void refresh() {};

	// Setzen von CV im Geraet
	virtual int setSM( int bus, int addr, int device, int cv, int value ) { return( 200 ); }
	virtual int getSM( int bus, int addr, int device, int cv ) { return( -1 ); };

	// Power ON/OFF
	virtual void setPower( int on ) {}
};

} /* namespace srcp */
#endif /* SRCPDEVICE_H_ */
