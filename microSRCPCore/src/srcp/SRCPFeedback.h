/*
	SRCPFeedback - Interface wo alle Funktionen definiert die
	ein Feed Back (Rueckmelder, Sensor) Geraet implementieren muss.

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

#ifndef SRCPFEEDBACK_H_
#define SRCPFEEDBACK_H_

#include <Arduino.h>
#include "SRCPDevice.h"

namespace srcp
{

struct fb_t
{
	union
	{
		uint8_t b;
		struct
		{
			unsigned pin : 7;
			unsigned value : 1;
		};
	};
} ;

typedef fb_t feedback;

// Globaler IO-Buffer fuer Sensoren - Platz fuer 8 Sensoren und 9 als Endmerker \0
extern feedback fb[9];

class SRCPFeedback: public srcp::SRCPDevice
{
protected:
	uint8_t oldSensor;
	uint8_t sensor;
private:
	SRCPFeedback* next;
public:
	SRCPFeedback* nextElement() { return ( next ); }
	void setNextElement( SRCPFeedback* next ) { this->next = next; }

	virtual int checkAddr( int addr ) { return( addr >= this->addr && addr <= (this->addr+8-1)  ); }
	virtual void refresh() {};
	virtual uint8_t get( int addr ) { oldSensor = sensor; sensor = 0; return ( oldSensor); }
	virtual int getEndAddr() { return( this->addr + 8-1 ); }
};
}

#endif /* SRCPFEEDBACK_H_ */
