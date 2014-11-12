/*
	GLOpenDCC - Steuerung von Lokomotiven
	anhand des DCC Signals.

	Copyright (c) 2010 Marcel Bernet.  All right reserved.

	Der Code wurde ermoeglicht durch das OpenDCC Projekt und
	die Zurverfuegungstellung der Sourcen von Wolfgang Kufer.
	Besten Dank!

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

extern "C"
{
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>

#include "hardware.h"
#include "config.h"                // general structures and definitions - make your changes here
#include "parser.h"
#include "dccout.h"                // make dcc
#include "organizer.h"             // manage commands
}
#include "GLOpenDCC.h"

namespace odcc
{

GLOpenDCC::GLOpenDCC( int startAddr, int endAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
}

int GLOpenDCC::set( int addr, int drivemode, int v, int v_max, int fn[] )
{
	// es sind nur Fahrstufen 0 - 127 moeglich, groessere Geschwindkeiten werden halbiert.
	if	( v_max > 127 )
	{
		v /= 2;
		v_max /= 2;
	}
	v = map( v, 0, v_max, 0, 127 );

	if	( drivemode )
		v |= 0x80;

	int f = bitRead( fn[3], 0 );
	do_loco_func_grp0( addr, f );	// Licht
	f = 0;
	for	( int i = 1; i < 5; i++ )
		bitWrite( f, i-1, bitRead(fn[3], i));
	do_loco_func_grp1( addr, f ); // F1 - F4

	// TODO F5 - F12 abhandeln
	do_loco_speed( addr, v );

	return	( 200 );
}

void GLOpenDCC::setPower( int on )
{
	if	( on == 0 )
	{
		// TODO warum funktioniert On/Off via Pin nicht?
		MAIN_TRACK_ON;
		organizer_halt_state = 0;
	}
	else
	{
		// TODO warum funktioniert On/Off via Pin nicht?
		MAIN_TRACK_OFF;
		do_all_stop();
	}
}

}
