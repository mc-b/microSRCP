/*
	GABlinkLed - blinkende Led

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

#include "GABlinkLed.h"
#include <Streaming.h>

namespace dev
{

GABlinkLed::GABlinkLed( int addr, uint8_t pin, int delay )
{
	this->addr = addr;
	this->pin = pin;
	this->delay = delay;

	pinMode( this->pin, OUTPUT );
	digitalWrite( this->pin, LOW );
	on = false;
	value = 0;
	last = millis();
}

int GABlinkLed::set( int addr, int port, int value, int delay )
{
	this->value = value;

	if	( value == 0 )
	{
		digitalWrite( this->pin, LOW );
		on = false;
	}
	else
		refresh();

	return	( 200 );
}

void GABlinkLed::refresh()
{
	if	( value )
	{
		if	( last + delay < millis() )
		{
			if	( on )
			{
				digitalWrite( this->pin, LOW );
				on = false;
			}
			else
			{
				digitalWrite( this->pin, HIGH );
				on = true;
			}
			last = millis();
		}
	}
}

}
