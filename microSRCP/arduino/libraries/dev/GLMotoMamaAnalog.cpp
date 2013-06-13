/*
	GLMotoMamaAnalog - Motorenansteuerung fuer Shields mit
	getrennten Richtungspins, z.B. MotoMama

	Beispiel fuer ein Board:
	http://blog.iteadstudio.com/ln298-h-bridge-moto-driver-shield-motomama/

	ACHTUNG: beim AtMega328 kann der 1. Kanal (Pin 8 - 11) nicht zusammen
	mit Servo's verwendet werden.

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

#include "GLMotoMamaAnalog.h"

namespace dev
{

GLMotoMamaAnalog::GLMotoMamaAnalog( int addr, uint8_t pin, uint8_t back, uint8_t ahead )
{
	this->addr = addr;
	this->pin = pin;
	this->back = back;
	this->ahead = ahead;

	pinMode(pin, OUTPUT);
	pinMode(back, OUTPUT);
	pinMode(ahead, OUTPUT);

	digitalWrite(back, LOW);
	digitalWrite(ahead, HIGH);
	analogWrite(pin, 0);
}

int GLMotoMamaAnalog::set(int addr, int drivemode, int v, int v_max, int fn[])
{
	// rueckwarts?
	if	( drivemode == 0 )
	{
		digitalWrite(back, HIGH);
		digitalWrite(ahead, LOW);
	}
	// vorwarets
	else
	{
		digitalWrite(back, LOW);
		digitalWrite(ahead, HIGH);
	}

	if	( v > v_max )
		v = v_max;
	if	( v < 0 )
		v = 0;
	v = map(v, 0, v_max, 0, 255);
	analogWrite( pin, v );
	return	( 200 );
}

void GLMotoMamaAnalog::setPower( int on )
{
	if	( on == 0 )
		analogWrite( pin, 0 );
}

}
