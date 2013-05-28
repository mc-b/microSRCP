/*
	GLAnalog293 - Sehr einfacher Motorentreiber kann an jedem
	PWM Ausgang mit einem Nachgeschalteten L283D o.ae.
	verwendet werden.
	Motor ist wegen dem PWM relativ laut, sollte nicht verwendet werden.

	Beispiel fuer ein Board:
	http://www.rn-wissen.de/index.php/RN-Control

	ACHTUNG: das die Arduino alle PWM Register neu initialisiert
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

#ifndef GL293_H_
#define GL293_H_

#include <Arduino.h>
#include "../srcp/SRCPGenericLoco.h"

/**
 * Analog Loksteuerung mit TEXAS INSTRUMENTS L293NEE4
 *
 * Darf nicht zusammen mit Servo verwendet werden, weil Servo alle PWM deaktiviert!
 *
 * Evtl. direkt PWM-Register ansprechen, mit Beispiel rn-control laeuft der Motor leiser!
 */
namespace dev
{

class GLAnalog293: public srcp::SRCPGenericLoco
{
private:
	uint8_t pin;
	uint8_t back;
	uint8_t ahead;
public:
	GLAnalog293( int addr, uint8_t pin, uint8_t back, uint8_t ahead );
	int set( int addr, int drivemode, int v, int v_max, int fn[] );
	void setPower( int on );
};

}

#endif /* GL293_H_ */
