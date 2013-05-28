/*
	GASignal - Einfache 2 begriffige Lichtsignal Ansteuerung.

	Ich verwende Signale von BEMO mit 2 LED die haben eine
	gemeinsame Leitung nach 5+V und je eine Leitung nach GND.

	Dadurch leuchtet die LED wenn der Pin auf GND geschaltet wird.

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

#include "GASignal.h"
#include <Streaming.h>

namespace dev
{

GASignal::GASignal( int addr, uint8_t red, uint8_t green )
{
	this->addr = addr;
	this->greenPin = green;
	this->redPin = red;

	pinMode( greenPin, OUTPUT );
	pinMode( redPin, OUTPUT );

	digitalWrite( redPin, HIGH );
	digitalWrite( greenPin, LOW );
}

int GASignal::set( int addr, int port, int value, int delay )
{
	if	( value == 0 )
	{
		digitalWrite( redPin, HIGH );
		digitalWrite( greenPin, LOW );
	}
	else
	{
		digitalWrite( redPin, LOW );
		digitalWrite( greenPin, HIGH );
	}

	return	( 200 );
}

}
