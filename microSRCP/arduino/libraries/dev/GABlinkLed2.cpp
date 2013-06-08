/*
	GABlinkLed2 - 2 LED's geschaltet gegen GND mit Implementierter
	Wechselblinker-Schaltung.

	In RocRail als Signal eintragen und bei Schnittstelle Weiche
	aktivieren. Das Geraet muss mit Adresse und Port = 0
	angesprochen werden, weil nicht value sondern port
	zum Umschalten von RocRail geschickt wird.

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

#include "GABlinkLed2.h"
#include <Streaming.h>

namespace dev
{

GABlinkLed2::GABlinkLed2( int addr, uint8_t pin1, uint8_t pin2, int delay )
{
	this->addr = addr;
	this->pin1 = pin1;
	this->pin2 = pin2;
	this->delay = delay;

	pinMode( this->pin1, OUTPUT );
	digitalWrite( this->pin1, LOW );
	pinMode( this->pin2, OUTPUT );
	digitalWrite( this->pin2, LOW );
	on = false;
	value = 0;
	last = millis();
}

int GABlinkLed2::set( int addr, int port, int value, int delay )
{
	this->value = port;

	if	( this->value == 0 )
	{
		digitalWrite( this->pin1, LOW );
		digitalWrite( this->pin2, LOW );
		on = false;
	}
	else
		refresh();

	return	( 200 );
}

void GABlinkLed2::refresh()
{
	if	( value )
	{
		if	( last + delay < millis() )
		{
			if	( on )
			{
				digitalWrite( this->pin1, LOW );
				digitalWrite( this->pin2, HIGH );
				on = false;
			}
			else
			{
				digitalWrite( this->pin1, HIGH );
				digitalWrite( this->pin2, LOW );
				on = true;
			}
			last = millis();
		}
	}
}

}
