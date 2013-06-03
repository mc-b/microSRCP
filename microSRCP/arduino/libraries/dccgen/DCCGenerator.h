/*
	DCCGenerator - Erzeugen des DCC Signals.

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

#ifndef DCCGENERATOR_H_
#define DCCGENERATOR_H_

#include <Arduino.h>

#define LONG delayMicroseconds( 100 );
#define SHORT delayMicroseconds( 58 );

// Port B an AtMega 328P
#define DCC_PIN_1_1 8
#define DCC_PIN_1_2 9
#define ENABLE_PIN1 10
#define ENABLE_PIN2 11
#define DCC_PIN_2_1 12
#define DCC_PIN_2_2 13

// Bits fuer do_one() - do_zero()
#define OUTPUT_STATE_1 B00011101;
#define OUTPUT_STATE_2 B00101110;

// Linked lists
struct _loco
{
	int address;
	byte speed; // This is the speed to pass directly in the DCC command.
	byte functions;

	struct _loco *next;
};
typedef struct _loco Loco;

struct _point
{
	int address;
	boolean straight;

	struct _point *next;
};
typedef struct _point Point;


class DCCGenerator
{
public:
	DCCGenerator() {};
	void setup();
	void doInstructions();
	void doReadback();
	void setPoint( int addr, byte s );
	void setSpeed( int addr, byte s );
	void setFunction( int addr, byte func, byte status );
    void directModeWriteByte( byte cv, byte value, int count = 3 );
private:
    // TODO Array fuer emergy, commands und repeat Befehle analog OpenDCC
	Loco *locos;
	Loco *firstLoco;
	Point *points;
	Point *firstPoint;
	void do_preamble();
	void do_byte( byte a );
	void do_one();
	void do_zero();
	void do_idle( int count = 1 );
	void do_reset( int count = 1 );
};

extern DCCGenerator DCC;

#endif /* DCCGENERATOR_H_ */
