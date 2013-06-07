/*
	GAWave - Abspielen einer Wave Datei.

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
#include <HardwareSerial.h>
#include <Streaming.h>
#include "WaveHC.h"

#include "GAWave.h"
#include "WaveDeviceManager.h"

// Debugging > 0 == ON
#define DEBUG_SCOPE 2

namespace wav
{

GAWave::GAWave( int addr, char* file)
{
	this->addr = addr;

	strcpy( this->file, file );
	strcat( this->file, ".WAV" );
}

int GAWave::set( int addr, int port, int value, int delay )
{
#if	( DEBUG_SCOPE > 0 )
	Serial.print( "play" );
	Serial.println( file );
#endif

	// nur Namen setzen, noch nicht abspielen, weil in Interrupt-Routine!
	WaveDevManager.setSound( file );
	return	( 200 );
}

}
