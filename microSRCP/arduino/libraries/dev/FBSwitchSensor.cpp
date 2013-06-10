/*
	FBSwitchSensor - Einfacher Sensor an einem Digitalen Eingang
	Der interne Pullup Wiederstand wird aktiviert, dadurch muss
	der Sensor gegen GND geschaltet werden um aktiv zu sein.

	Mittels refresh werden die Sensoren lokal abgefragt, ein
	aktiver Sensor wird gespeichert und erst bei info oder
	get wieder zurueckgesetzt.

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

#include "FBSwitchSensor.h"
#if	( DEBUG_SCOPE > 1 )
#include <Streaming.h>
#endif

namespace dev
{

FBSwitchSensor::FBSwitchSensor( int addr, uint8_t startPin, uint8_t endPin )
{
	this->addr = addr;
	this->startPin = startPin;
	this->endPin = endPin;

	for	( int i = startPin; i <= endPin; i++ )
		pinMode( i, INPUT_PULLUP );

	oldSensor = 0;
	sensor = 0;
}

void FBSwitchSensor::refresh( )
{
	// alle Werte abfragen, nur der Wechsel auf LOW fuehrt zu Aenderung des Sensorstatuses
	for	( int i = startPin; i <= endPin; i++ )
	{
		int v = digitalRead( i ) == 0;
		if	( v )
			bitSet( sensor, i - startPin );
	}
}

int FBSwitchSensor::info( int addr, srcp::feedback fb[] )
{
	refresh();

	memset( fb, 0, 9 );

	int pos = 0;
	for	( int i = 0; i < 8; i++ )
		if	( bitRead(oldSensor, i) != bitRead(sensor,i) )
		{
			fb[pos].pin = i + this->addr;
			fb[pos].value = bitRead(sensor, i);
#if	( DEBUG_SCOPE > 1 )
			Serial << "sensor " << fb[pos].pin << ":" << fb[pos].value << endl;
#endif
			pos++;
		}

	oldSensor = sensor;
	sensor = 0;
	return	( 0 );
}

}
