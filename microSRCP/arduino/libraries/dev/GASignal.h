/*
	GASignal - Einfache 2 begriffige Lichtsignal Ansteuerung.

	Ich verwende Signale von BEMO mit 2 LED die haben eine
	gemeinsame Leitung nach 5+V und je eine Leitung nach GND.

	Dadurch leuchtet die LED wenn der Pin auf GND geschaltet wird.

	Ein Signal hat eine Adresse und mittels den Ports werden die
	verschiedenen Led's angesteuert. Die Ports werden von 0
	an adressiert, d.h. Port 0 = StartPin, Port 1 = 2. Led etc.

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

#ifndef GASIGNAL_H_
#define GASIGNAL_H_

#include <inttypes.h>
#include <Arduino.h>
#include "../srcp/SRCPGenericAccessoire.h"

namespace dev
{

class GASignal: public srcp::SRCPGenericAccessoire
{
private:
	uint8_t startPin;
	uint8_t endPin;
public:
	GASignal( int addr, uint8_t start, uint8_t end );
	int get( int addr, int port ) { return ( 200 ); }
	int set( int addr, int port, int value, int delay );
};

}

#endif /* GASIGNAL_H_ */
