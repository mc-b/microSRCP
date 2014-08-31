/*
	GASignalMatrix - Einfache 2 begriffige Lichtsignal Ansteuerung.

	Ich verwende Signale von BEMO mit 2 LED die haben eine
	gemeinsame Leitung nach 5+V und je eine Leitung nach GND.

	Dadurch leuchtet die LED wenn der Pin auf GND geschaltet wird.

	Ein Signal hat eine Adresse und mittels den Ports werden die
	verschiedenen Led's angesteuert. Die Ports werden von 0
	an adressiert, d.h. Port 0 = StartPin, Port 1 = 2. Led etc.

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

#ifndef GASIGNALMATRIX_H_
#define GASIGNALMATRIX_H_

#include <inttypes.h>
#include <Arduino.h>
#include "../srcp/SRCPGenericAccessoire.h"

#define max7219_reg_noop        0x00
#define max7219_reg_digit0      0x01
#define max7219_reg_digit1      0x02
#define max7219_reg_digit2      0x03
#define max7219_reg_digit3      0x04
#define max7219_reg_digit4      0x05
#define max7219_reg_digit5      0x06
#define max7219_reg_digit6      0x07
#define max7219_reg_digit7      0x08
#define max7219_reg_decodeMode  0x09
#define max7219_reg_intensity   0x0a
#define max7219_reg_scanLimit   0x0b
#define max7219_reg_shutdown    0x0c
#define max7219_reg_displayTest 0x0f

namespace dev
{

class GASignalMatrix: public srcp::SRCPGenericAccessoire
{
private:
	uint8_t load;    	// CS pin of MAX7219 module
	uint8_t clock;  	// CLK pin of MAX7219 module
	uint8_t data;    	// DIN pin of MAX7219 module
	int startAddr;
	int endAddr;
    byte buffer[8];
	void reload();

public:
	GASignalMatrix(  int startAddr, int endAddr, uint8_t cs = 5, uint8_t clk = 6, uint8_t din = 7 );
	int checkAddr( int addr ) { return ( addr >= startAddr && addr <= endAddr); }
	int get( int addr, int port ) { return ( 200 ); }
	int set( int addr, int port, int value, int delay );

	void init();
	void clear();
	void setCommand( byte command, byte value );
	void setIntensity( byte intensity );
	void setColumnAll( byte col, byte value );
	void setDot( byte col, byte row, byte value );
};

}

#endif /* GASIGNALMATRIX_H_ */
