/*
	GASignal - Einfache 2 begriffige Lichtsignal Ansteuerung.

	Ich verwende Signale von BEMO mit 2 LED die haben eine
	gemeinsame Leitung nach 5+V und je eine Leitung nach GND.

	Dadurch leuchtet die LED wenn der Pin auf GND geschaltet wird.

	Ein Signal hat eine Adresse und mittels den Ports werden die
	verschiedenen Led's angesteuert. Die Ports werden von 0
	an adressiert, d.h. Port 0 = StartPin, Port 1 = 2. Led etc.

	Copyright (c) 2010 - 2013 Marcel Bernet.  All right reserved.

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

#include "GASignal.h"

namespace dev
{

GASignal::GASignal( int addr, uint8_t start, uint8_t end )
{
	this->addr = addr;
	startPin = start;
	endPin = end;

	for	( int i = startPin; i <= endPin; i++ )
		pinMode( i, OUTPUT );

	digitalWrite( startPin, HIGH );
	digitalWrite( endPin, LOW );
}

int GASignal::set( int addr, int port, int value, int delay )
{
	int pin = startPin + port;
	if	( pin > endPin )
		pin = endPin;

	if	( value == 0 )
		digitalWrite( pin, LOW );
	else
		digitalWrite( pin, HIGH );

	return	( 200 );
}


}
