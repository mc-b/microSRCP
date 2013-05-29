/*
	GAServo - Einfache Servo Ansteuerung mittels der Arduino
	Servo Library.

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

#include "GAServo.h"

namespace dev
{
GAServo::GAServo( int addr, uint8_t pin, uint8_t min, uint8_t max )
{
	servo = (Servo*) 0;

	this->pin = pin;
	this->addr = addr;
	this->min = min;
	this->max = max;

	servo = new Servo();
	servo->attach( pin );
	servo->write( min );
}

int GAServo::set( int addr, int port, int value, int delay )
{
	if	( value == 0 )
		servo->write( min );
	else
		servo->write( max );

	return	( 200 );
}

}
