/*
	GLArduinoMotor - Motorentreiber fuer das Original Arduino
	Motortreiber Board.

	Shield:
	http://arduino.cc/en/Main/ArduinoMotorShield oder
	http://www.nkcelectronics.com/freeduino-arduino-motor-control-shield-kit.html

	ACHTUNG: das Arduino alle PWM Register neu initialisiert
	kann dieser Treiber nicht mit Servos zusammen verwendet werden.

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
#include "GLArduinoMotor.h"

namespace dev
{

GLArduinoMotor::GLArduinoMotor( int addr, uint8_t pin, uint8_t dir )
{
	this->addr = addr;
	this->pin = pin;
	this->dir = dir;

	pinMode( dir, OUTPUT );
	digitalWrite( dir, HIGH );

	// ohne Funktion?
	//setPwmFrequency( pin, 1024 );
	pinMode( pin, OUTPUT );
}

int GLArduinoMotor::set( int addr, int drivemode, int v, int v_max, int fn[] )
{
	// rueckwarts?
	if ( drivemode == 0 )
		digitalWrite( dir, LOW );
	// vorwarets
	else
		digitalWrite( dir, HIGH );

	if ( v > v_max )
		v = v_max;
	if ( v < 0 )
		v = 0;

	v = map( v, 0, v_max, 0, 255 );
	analogWrite( pin, v );

	return (200);
}

void GLArduinoMotor::setPower( int on )
{
	if	( on == 0 )
		analogWrite( pin, 0 );
}

/**
	// Set pin 9's PWM frequency to 3906 Hz (31250/8 = 3906)
	// Note that the base frequency for pins 3, 9, 10, and 11 is 31250 Hz
	setPwmFrequency(9, 8);

	// Set pin 6's PWM frequency to 62500 Hz (62500/1 = 62500)
	// Note that the base frequency for pins 5 and 6 is 62500 Hz
	setPwmFrequency(6, 1);

	// Set pin 10's PWM frequency to 31 Hz (31250/1024 = 31)
	setPwmFrequency(10, 1024);
 */

/**
 * Divides a given PWM pin frequency by a divisor.
 *
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 *
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired.
 *   - Pins 9 and 10 are paired.
 *   - Pins 3 and 11 are paired.
 *
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 *
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void GLArduinoMotor::setPwmFrequency( int pin, int divisor )
{
	byte mode;
	if ( pin == 5 || pin == 6 || pin == 9 || pin == 10 )
	{
		switch ( divisor )
		{
		case 1:
			mode = 0x01;
			break;
		case 8:
			mode = 0x02;
			break;
		case 64:
			mode = 0x03;
			break;
		case 256:
			mode = 0x04;
			break;
		case 1024:
			mode = 0x05;
			break;
		default:
			return;
		}
		if ( pin == 5 || pin == 6 )
		{
			TCCR0B = TCCR0B & 0b11111000 | mode;
		}
		else
		{
			TCCR1B = TCCR1B & 0b11111000 | mode;
		}
	}
	else if ( pin == 3 || pin == 11 )
	{
		switch ( divisor )
		{
		case 1:
			mode = 0x01;
			break;
		case 8:
			mode = 0x02;
			break;
		case 32:
			mode = 0x03;
			break;
		case 64:
			mode = 0x04;
			break;
		case 128:
			mode = 0x05;
			break;
		case 256:
			mode = 0x06;
			break;
		case 1024:
			mode = 0x7;
			break;
		default:
			return;
		}
		TCCR2B = TCCR2B & 0b11111000 | mode;
	}
}

}
