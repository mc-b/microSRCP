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

#ifndef GASLOWSERVO_H_
#define GASLOWSERVO_H_

#include "../srcp/SRCPGenericAccessoire.h"
#include <Servo.h>

namespace dev
{

class GASlowServo: public srcp::SRCPGenericAccessoire
{
private:
	Servo *servo;
	uint8_t pin;
	uint8_t min;
	uint8_t max;
	uint8_t current;	// aktuelle Position
	uint8_t pos;	// gewuenschte Position
	uint8_t step;	// Einheiten um wieviel jeweils weitergeschaltet werden soll
	int delay;		// Pausen zwischen der Weiterschaltung
	unsigned long last;
public:
	GASlowServo( int addr, uint8_t pin, uint8_t min, uint8_t max, uint8_t step = 1, int delay = 50 );
	int get( int addr, int port ) { return ( 200 ); }
	int set( int addr, int port, int value, int delay );
	void refresh();
};

}

#endif /* GASLOWSERVO_H_ */
