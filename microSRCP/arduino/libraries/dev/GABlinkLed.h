/*
	GABlinkLed - einfache LED geschaltet gegen GND.

	Mehrere LED ergeben ein Signal. Das entspricht besser der
	Logik von RocRail, wo jeder Signalausgang einzeln geschaltet
	wird.

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

#ifndef GABLINKLED_H_
#define GABLINKLED_H_

#include <inttypes.h>
#include <Arduino.h>
#include "../srcp/SRCPGenericAccessoire.h"

namespace dev
{

class GABlinkLed: public srcp::SRCPGenericAccessoire
{
private:
	uint8_t pin;
	uint8_t on;					// aktueller Zustand
	uint8_t value;
	int delay;
	unsigned long last;
public:
	// Adresse, Pin, Pause zwischen dem Blinken
	GABlinkLed( int addr, uint8_t pin, int delay );
	int get( int addr, int port ) { return ( 200 ); }
	int set( int addr, int port, int value, int delay );
	void refresh();
};

}

#endif /* GABLINKLED_H_ */
