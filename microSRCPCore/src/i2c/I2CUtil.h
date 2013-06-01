/*
	I2CUtil - Hilfsklasse fuer den I2C Bus.
	Stellt LowLevel Funktionen wie write/read zur Verfuegung.
	Auf die Arduino Wire Library kann dadurch komplett verzichtet
	werden (spart Speicherplatz) und die Kommunikation ist
	sicherer durch Timeout und wiederholtes Senden bei Fehler.

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

#ifndef I2CUTIL_H_
#define I2CUTIL_H_

#include <Arduino.h>

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
  #include "twi.h"
}

namespace i2c
{

class I2CUtil
{
public:
	static void begin( int addr );
	static int write( int addr, uint8_t *buf, int size );
	static int read( int addr, uint8_t *buf, int size );
	static int setSM( int remoteAddr, int bus, int addr, int device, int cv, int value );
	static int getSM( int remoteAddr, int bus, int addr, int device, int cv );
	static int getDescription( int remoteAddr, int bus, int addr, int device, uint8_t *rc );
};

}

#endif /* I2CUTIL_H_ */
