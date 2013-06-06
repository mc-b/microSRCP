/*
	SRCPFeedback - Interface wo alle Funktionen definiert die
	ein Generic Accessoire (Servo, Lichtsignal ...) Geraet
	implementieren muss.

	Siehe auch: http://srcpd.sourceforge.net/srcp/

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

#ifndef SRCPGENERICACCESSOIRE_H_
#define SRCPGENERICACCESSOIRE_H_

#include "SRCPDevice.h"

namespace srcp
{
class SRCPGenericAccessoire: public srcp::SRCPDevice
{
private:
	SRCPGenericAccessoire* next;
public:
	SRCPGenericAccessoire* nextElement() { return ( next ); }
	void setNextElement( SRCPGenericAccessoire* next ) { this->next = next; }
	virtual void refresh() {};

	// Setzen von Geraete Eigenschaften
	virtual int get( int addr, int port ) { return ( 423 ); }
	virtual int set( int addr, int port, int value, int delay ) { return ( 423 ); }

	// Power ON/OFF
	virtual void setPower( int on ) {}
};

}

#endif /* SRCPGENERICACCESSOIRE_H_ */
