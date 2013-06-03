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

#include "DCCGenerator.h"

/**
 * Initialisierung des DCC Shields
 */
void DCCGenerator::setup()
{
	locos = (Loco *) 0;
	firstLoco = (Loco *) 0;
	points = (Point *) 0;
	firstPoint = (Point *) 0;

	pinMode( DCC_PIN_1_1, OUTPUT );
	pinMode( DCC_PIN_1_2, OUTPUT );
	pinMode( ENABLE_PIN1, OUTPUT );
	pinMode( ENABLE_PIN2, OUTPUT );
	pinMode( DCC_PIN_2_1, OUTPUT );
	pinMode( DCC_PIN_2_2, OUTPUT );
}


/**
 * Stellt ein Zubehoerartikel
 * TODO sind damit Zubehoerartikel gemeint?
 */
void DCCGenerator::setPoint( int addr, byte s )
{
	for ( Point *curPoint = firstPoint; curPoint; curPoint = curPoint->next )
	{
		if ( curPoint->address == addr )
		{
			curPoint->straight = (s > 0);
			return;
		}
	}

	// Didn't find an existing point with that address, so create a new one
	// and stick it at the front of the queue.
	Point *newPoint = (Point *) malloc( sizeof(Point) * 1 );
	newPoint->address = addr;
	newPoint->straight = (s > 0);
	newPoint->next = firstPoint;

	firstPoint = newPoint;
}

/**
 * Setzt die Geschwindigkeit einer Loco
 */
void DCCGenerator::setSpeed( int addr, byte s )
{
	for ( Loco *curLoco = firstLoco; curLoco; curLoco = curLoco->next )
	{
		if ( curLoco->address == addr )
		{
			curLoco->speed = s;
			//Serial << (int) curLoco->address << " " << (int) curLoco->speed << endl;
			return;
		}
	}

	// Didn't find an existing loco with that address, so create a new one
	// and stick it at the front of the queue.
	Loco *newLoco = (Loco *) malloc( sizeof(Loco) * 1 );
	newLoco->address = addr;
	newLoco->speed = s;
	newLoco->next = firstLoco;
	//Serial << (int) newLoco->address << " " << (int) newLoco->speed << endl;

	firstLoco = newLoco;
}

/**
 * Setzt die Funktionen einer Loco
 */
void DCCGenerator::setFunction( int addr, byte func, byte status )
{
	for ( Loco *curLoco = firstLoco; curLoco; curLoco = curLoco->next )
	{
		if ( curLoco->address == addr )
		{
			if ( status == 1 )
			{
				curLoco->functions |= 1 << (func - 1);
			}
			else
			{
				curLoco->functions &= ~(1 << (func - 1));
			}
			return;
		}
	}
}

/**
 * Programmiert ein CV auf dem Programmiergleis
 * Wird 3 mal durchlaufen, gibt groesste Sicherheit zum programmieren.
 */
void DCCGenerator::directModeWriteByte( byte cv, byte value, int count )
{
	// CV Register beginnen bei 0
	cv -= 1;

	for ( int i = 0; i < count; i++ )
	{
		// reset
		do_reset( 3 );

		// CV Programmierung - sollten eigentlich nur 5 sein, 10 sind aber sicherer
		for ( int i = 0; i < 10; i++ )
		{

			do_preamble();
			// 10 bits bilden die CV Adresse von 1 - 1024
			byte cmd = 0b01111100 | ((cv >> 8) & 0x03);
			cv = cv & 0xFF;
			do_byte( cmd );
			do_byte( cv );
			do_byte( value );
			do_byte( cmd ^ cv ^ value );
			do_one();
		}
		// Decoder Recovery-Time
		// FIXME ACK Signal, z.B. an Analog Pin abhandeln
		// @see http://www.die-maeusis.de/lokprogrammer.htm
		delay( 6 );
		do_reset( 6 );
	}
}

/**
 * Sendet x Resetpackages
 */
void DCCGenerator::do_reset( int count )
{
	for	( int i = 0; i < count; i++ )
	{
		do_preamble();
		do_byte( 0x00 );
		do_byte( 0x00 );
		do_byte( 0x00 ^ 0x00 );
		do_one();
	}
}

/**
 * Sendet count Idle Packages
 */
void DCCGenerator::do_idle( int count )
{
	for ( int i = 0; i < count; i++ )
	{
		do_preamble();
		do_byte( 0xff );
		do_byte( 0x00 );
		do_byte( 0xff );
		do_one();
	}
}

/**
 * Sendet und refresht die Signale am Gleis
 */
void DCCGenerator::doInstructions()
{
	for ( Loco *curLoco = firstLoco; curLoco; curLoco = curLoco->next )
	{
		if ( curLoco->address )
		{
			// kurze DCC Adresse
			if	( curLoco->address < 128 )
			{
				// Loco Befehl als Extended Package mit 126 Fahrstufen zusammenstellen
				// 0 AAAAAAAA 0 CCCDDDDD 0 XSSSSSSS 0 EEEEEEEE 1
				// A = Adresse
				// C = Befehle hier 1 = Advanced Operation Instructions
				// D = 5 Data Bits immer 11111
				// X = Fahrrichtung, 0 = rueckwaerts, 1 = vorwaerts
				// S = Fahrstufe 2 - 126, 0 = Stop, 1 = Nothalt
				do_preamble();
				byte ad = curLoco->address & 0xFF;
				do_byte( ad );
				do_byte( 0x3f );
				do_byte( curLoco->speed );
				do_byte( ad ^ 0x3f ^ curLoco->speed );
				do_one();

				// Funktion Befehl fuer Loco
				// D = 100 Function Group One Instruction F0 - F4
/*				do_preamble();
				do_byte( curLoco->address );
				// FIXME Funktion F5 - ... abhandeln
				do_byte( 128 + curLoco->functions );
				do_byte( curLoco->address ^ (128 + curLoco->functions) );
				do_one();*/
			}
			// Lange DCC Adresse
			else
			{
				// Loco Befehl als Extended Package mit 126 Fahrstufen zusammenstellen
			    // message 11AAAAAA AAAAAAAA 00111111 DSSSSSSS    ; Speed comes msb first
				// A = Adresse
				// C = Befehle hier 1 = Advanced Operation Instructions
				// D = 5 Data Bits immer 11111
				// X = Fahrrichtung, 0 = rueckwaerts, 1 = vorwaerts
				// S = Fahrstufe 2 - 126, 0 = Stop, 1 = Nothalt
				do_preamble();
				byte a1 = 0xC0 | ( (unsigned char)(curLoco->address / 256) & 0x3F);
				byte a2 = (char)(curLoco->address & 0xFF);
				do_byte( a1 );
				do_byte( a2 );
				do_byte( 0x3f );
				do_byte( curLoco->speed );
				do_byte( a1 ^ a2 ^ 0x3f ^ curLoco->speed );
				do_one();

				// Funktion Befehl fuer Loco
				// D = 100 Function Group One Instruction F0 - F4
/*				do_preamble();
				do_byte( curLoco->address );
				// FIXME Funktion F5 - ... abhandeln
				do_byte( 128 + curLoco->functions );
				do_byte( curLoco->address ^ (128 + curLoco->functions) );
				do_one();*/
			}
		}
	}

	for ( Point *curPoint = firstPoint; curPoint; curPoint = curPoint->next )
	{
		if ( curPoint->address )
		{
			int channel = (((curPoint->address - 1) & 0x03) << 1);
			if ( curPoint->straight )
			{
				channel |= 1;
			}

			int address = (curPoint->address - 1) >> 2;
			address += 1;

			int a = 0x80 | (address & 0x3f);
			int b = 0x80 | ((((~address) >> 6) & 0x07) << 4) | 0x08 | (channel & 0x07);
            do_preamble();
            do_byte(a);
            do_byte(b);
            do_byte(a ^ b);
            do_one();
        }

    }

    do_idle();
}

/**
 * Platzhalter fuer evtl. Auswertung von BIDI Befehlen
 */
void DCCGenerator::doReadback()
{
	SHORT;
	SHORT;
	SHORT;
	SHORT;
}

/**
 * Einleitendes HIGH Bit fuer Decoder senden
 */
void DCCGenerator::do_preamble()
{
	for ( int i = 0; i < 20; i++ )
	{
		do_one();
	}
}

/**
 * Bit 0 XXXXXXXX mit fuehrendem LOW Bit senden
 */
void DCCGenerator::do_byte( byte a )
{
	int t = 128;

	do_zero();

	for ( int b = 0; b <= 7; b++ )
	{
		if ( a & t )
		{
			do_one();
		}
		else
		{
			do_zero();
		}

		t /= 2;
	}
}

/**
 * LOW Bit senden
 */
void DCCGenerator::do_zero()
{
	PORTB = OUTPUT_STATE_1;
	LONG;

	PORTB = OUTPUT_STATE_2;
	LONG;
}

/**
 * HIGH Bit senden
 */
void DCCGenerator::do_one()
{
	PORTB = OUTPUT_STATE_1;
	SHORT;

	PORTB = OUTPUT_STATE_2;
	SHORT;
}

DCCGenerator DCC = DCCGenerator();
