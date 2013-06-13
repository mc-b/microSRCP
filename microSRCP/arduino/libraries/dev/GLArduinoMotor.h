/*
	GLArduinoMotor - Motorentreiber fuer untenstehende Shields

	Shield:
	http://arduino.cc/en/Main/ArduinoMotorShieldR3 oder
	http://www.nkcelectronics.com/freeduino-arduino-motor-control-shield-kit.html

	ACHTUNG: je nach Pinbelegug des Shields (9 und 10 sind problematisch)
	kann der Code nicht zusammen mit Servo's verwendet werden.

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

#ifndef GLARDUINOMOTOR_H_
#define GLARDUINOMOTOR_H_

#include "../srcp/SRCPGenericLoco.h"

namespace dev
{

class GLArduinoMotor : public srcp::SRCPGenericLoco
{
private:
	uint8_t pin;
	uint8_t dir;
public:
	GLArduinoMotor( int addr, uint8_t pin, uint8_t dir );
	int set( int addr, int drivemode, int v, int v_max, int fn[] );
	void setPower( int on );
};

}

#endif /* GLARDUINOMOTOR_H_ */
