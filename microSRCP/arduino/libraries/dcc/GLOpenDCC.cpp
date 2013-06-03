/*
	GLOpenDCC - Steuerung von Lokomotiven
	anhand des DCC Signals.

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

#include "GLOpenDCC.h"
#include <Arduino.h>
#include "../dccgen/DCCGenerator.h"
#include <Streaming.h>

namespace dcc
{

GLOpenDCC::GLOpenDCC( int startAddr, int endAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
}

int GLOpenDCC::set( int addr, int drivemode, int v, int v_max, int fn[] )
{
	// es sind nur Fahrstufen 0 - 127 moeglich, groessere Geschwindkeiten werden halbiert.
/*	if	( v_max > 127 )
	{
		v /= 2;
		v_max /= 2;
	}
	v = map( v, 0, v_max, 0, 127 );*/

	if	( drivemode )
		v = (127 - v) * -1;

#if	( DEBUG_SCOPE > 10 )
	Serial3 << "setGL addr=" << addr << ", v=" << v << ", f0=" << fn[0] << ", f1-f4=" << fn[1] << endl;
#endif
	DCC.setSpeed( addr, (char) v );
	DCC.setFunction( addr, 1, fn[0] );
	return	( 200 );
}

void GLOpenDCC::setPower( int on )
{
}

int GLOpenDCC::setSM( int bus, int addr, int device, int cv, int value )
{
#if	( DEBUG_SCOPE > 10 )
	Serial3 << "write CV: " << (int) addr << " " << (int) cv << " " << (int) value << endl;
#endif

	DCC.directModeWriteByte( cv, value );
	return( 200 );
}

int GLOpenDCC::getSM( int bus, int addr, int device, int cv )
{
	return( 200 );
}

}
