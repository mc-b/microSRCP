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

#ifndef GLMOTOMAMA_ANALOG_H_
#define GLMOTOMAMA_ANALOG_H_

#include <Arduino.h>
#include "../srcp/SRCPGenericLoco.h"

/**
 * Analog Loksteuerung mit Moto Mama Shield
 *
 * Darf nicht zusammen mit Servo verwendet werden, weil Servo alle PWM deaktiviert!
 */
namespace dev
{

class GLMotoMamaAnalog: public srcp::SRCPGenericLoco
{
private:
	uint8_t pin;
	uint8_t back;
	uint8_t ahead;
public:
	GLMotoMamaAnalog( int addr, uint8_t pin, uint8_t back, uint8_t ahead );
	int set( int addr, int drivemode, int v, int v_max, int fn[] );
	void setPower( int on );
};

}

#endif /* GLMOTOMAMA_ANALOG_H_ */
