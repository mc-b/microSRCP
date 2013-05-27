/*
	GAPWMServo - Einfache Servo Ansteuerung auf den PWM Pins.
	Das sind bei AtMega328P die Pin 3, 5, 6, 9, 10, 11.
	Wird benoetigt um auf dem AtMega328P Servo parallel
	zur Motor Shield betreiben zu koennen.

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

#ifndef GAPWMSERVO_H_
#define GAPWMSERVO_H_

#include <Arduino.h>
#include "../srcp/SRCPGenericAccessoire.h"

namespace dev
{

class GAPWMServo: public srcp::SRCPGenericAccessoire
{
private:
	uint8_t pin;
	uint8_t min;
	uint8_t max;
public:
	GAPWMServo( int addr, uint8_t pin, uint8_t min, uint8_t max );
	int get( int addr, int port ) { return ( 200 ); }
	int set( int addr, int port, int value, int delay );

	// Power ON/OFF
	void setPower( int on );
};

}

#endif /* GAPWMSERVO_H_ */
