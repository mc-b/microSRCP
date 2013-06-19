/*
	GLDCCBooster - Steuerung von Lokomotiven
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

#include <Arduino.h>
#include "GLDCCBooster.h"
#include "../dccgen/DCCGenerator.h"
#include "../log/Logger.h"

namespace dcc
{

GLDCCBooster::GLDCCBooster( int startAddr, int endAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
}

int GLDCCBooster::set( int addr, int drivemode, int v, int v_max, int fn[] )
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

	DEBUG3( "setGL addr ", addr, v );

	DCC.setSpeed( addr, (char) v );
	DCC.setFunction( addr, 1, fn[0] );
	return	( 200 );
}

void GLDCCBooster::setPower( int on )
{
}

int GLDCCBooster::setSM( int bus, int addr, int device, int cv, int value )
{
	DEBUG3( "write CV ", addr, cv );

	DCC.directModeWriteByte( cv, value );
	return( 200 );
}

int GLDCCBooster::getSM( int bus, int addr, int device, int cv )
{
	return( 200 );
}

}
