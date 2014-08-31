/*
	GASignalMatrix - Einfache 2 begriffige Lichtsignal Ansteuerung.

	Ich verwende Signale von BEMO mit 2 LED die haben eine
	gemeinsame Leitung nach 5+V und je eine Leitung nach GND.

	Dadurch leuchtet die LED wenn der Pin auf GND geschaltet wird.

	Ein Signal hat eine Adresse und mittels den Ports werden die
	verschiedenen Led's angesteuert. Die Ports werden von 0
	an adressiert, d.h. Port 0 = StartPin, Port 1 = 2. Led etc.

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

#include "GASignalMatrix.h"

namespace dev
{

GASignalMatrix::GASignalMatrix( int startAddr, int endAddr, uint8_t cs, uint8_t clk, uint8_t din )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
	this->load = cs;
	this->clock = clk;
	this->data = din;

	// Zwischenbuffer mit allen Werten der LED's
	for ( int i = 0; i < 8; i++ )
		buffer[i] = 0;

	init();
}

void GASignalMatrix::init()
{
        pinMode(data,  OUTPUT);
        pinMode(clock, OUTPUT);
        pinMode(load,  OUTPUT);
        digitalWrite(clock, HIGH);

        setCommand(max7219_reg_scanLimit, 0x07);
        setCommand(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
        setCommand(max7219_reg_shutdown, 0x01);    // not in shutdown mode
        setCommand(max7219_reg_displayTest, 0x00); // no display test

        // empty registers, turn all LEDs off
        clear();

        setIntensity(0x0f);    // the first 0x0f is the value you can set
}

void GASignalMatrix::setIntensity(byte intensity)
{
	setCommand( max7219_reg_intensity, intensity );
}

void GASignalMatrix::clear()
{
	for ( int i = 0; i < 8; i++ )
		setColumnAll( i, 0 );

	for ( int i = 0; i < 8; i++ )
		buffer[i] = 0;
}

void GASignalMatrix::setCommand(byte command, byte value)
{
	digitalWrite( load, LOW );
	for ( int i = 0; i < 1; i++ )
	{
		shiftOut( data, clock, MSBFIRST, command );
		shiftOut( data, clock, MSBFIRST, value );
	}
	digitalWrite( load, LOW );
	digitalWrite( load, HIGH );
}

void GASignalMatrix::setColumnAll( byte col, byte value )
{
	digitalWrite( load, LOW );
	for ( int i = 0; i < 1; i++ )
	{
		shiftOut( data, clock, MSBFIRST, col + 1 );
		shiftOut( data, clock, MSBFIRST, value );
		buffer[col * i] = value;
	}
	digitalWrite( load, LOW );
	digitalWrite( load, HIGH );
}

void GASignalMatrix::setDot( byte col, byte row, byte value )
{
	bitWrite( buffer[col], row, value );

	int n = col / 8;
	int c = col % 8;
	digitalWrite( load, LOW );
	for ( int i = 0; i < 1; i++ )
	{
		if ( i == n )
		{
			shiftOut( data, clock, MSBFIRST, c + 1 );
			shiftOut( data, clock, MSBFIRST, buffer[col] );
		}
		else
		{
			shiftOut( data, clock, MSBFIRST, 0 );
			shiftOut( data, clock, MSBFIRST, 0 );
		}
	}
	digitalWrite( load, LOW );
	digitalWrite( load, HIGH );
}

int GASignalMatrix::set( int addr, int port, int value, int delay )
{
	int col = addr - startAddr;
	int row = port + 2;

	setDot( col, row, value );

	return	( 200 );
}


}
