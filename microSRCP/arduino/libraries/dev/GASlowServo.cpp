/*
	GASlowServo - Servo Ansteuerung mittels der Arduino
	Servo Library. Dabei bewegt sich der Anker des Servo's
	in der eingestellten Zeit und Schritten von der einen
	zur anderen Stellung. Nach Erreichen der gewuenschten
	Stellung wird die Stromzufuhr zum Servo abgeschaltet.

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

#include <Arduino.h>
#include "GASlowServo.h"

namespace dev
{
GASlowServo::GASlowServo( int addr, uint8_t pin, uint8_t min, uint8_t max, uint8_t step, int delay )
{
	servo = (Servo*) 0;

	this->pin = pin;
	this->addr = addr;
	this->min = min;
	this->max = max;
	this->step = step;
	this->delay = delay;

	servo = new Servo();

	// Trick - setzt den Servo auf die min Position
	pos = min;
	current = min + step;
	last = millis() + delay;
	servo->attach( pin );
	servo->write( min );
}

int GASlowServo::set( int addr, int port, int value, int delay )
{
	if	( value == 0 )
	{
		pos = min;
		last = millis();
		servo->attach( pin );

	}
	else
	{
		pos = max;
		last = millis();
		servo->attach( pin );
	}
	return	( 200 );
}

void GASlowServo::refresh()
{
	if	( last + delay < millis() )
	{
		last = millis();
		if	( current > pos )
		{
			current -= step;
			current = (current < min) ? min : current;
			servo->write( current );
		}
		else if ( current < pos )
		{
			current += step;
			current = (current > max) ? max : current;
			servo->write( current );
		}
		// else current == pos
		else
		{
			if	( servo->attached() )
				servo->detach();
		}
	}
}

}
