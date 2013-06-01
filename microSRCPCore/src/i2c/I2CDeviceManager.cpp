/*
	I2CDeviceManager - sucht den I2C Bus ab und erstellt lokale
	Proxies fuer die Remote Geraete. Damit wird eine moeglichst
	effiziente I2C Abhandlung gewaehrleistet.

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

#include <HardwareSerial.h>
#include <Streaming.h>
#include "I2CDeviceManager.h"
#include "I2CUtil.h"
#include "I2CFBMaster.h"
#include "I2CGAMaster.h"
#include "I2CGLMaster.h"
#include "../srcp/SRCPDeviceManager.h"

namespace i2c
{

#define CV_BOARD 		2

void I2CDeviceManager::begin( int startFB, int startGA, int reservedFB, int reservedGA )
{
	// I2C Master
	I2CUtil::begin( 0 );

	union
	{
		uint8_t byte[12];
		int values[6];
	} buf;

#if	( DEBUG_SCOPE > 0 )
	Serial3.println( "search I2C bus" );
#endif

	for	( int i = 1; i < 128; i++ )
	{
		int board = I2CUtil::getSM( i, 0, 0, srcp::CV, CV_BOARD );
		// kein I2C Board auf dieser Adresse vorhanden?
		if	( board == -1 )
		{
			// Platzhalter 8 Adressen freihalten
			startFB += reservedFB;
			startGA += reservedGA;
			continue;
		}
		// Liefert die von/bis von FB, GA, GL
		int rc = I2CUtil::getDescription( i, 0, 0, srcp::LAN, buf.byte );
		if	( rc == -1 )
			continue;

#if	( DEBUG_SCOPE > 0 )
			Serial3 << "I2C addr:id: " << i << ":" << board << ", fb: " << startFB << " " << buf.values[0] << "-" << buf.values[1] <<
					", ga: " << startGA << " " << buf.values[2] << "-" << buf.values[3] <<
					", gl: " << buf.values[4] << "-" << buf.values[5] << endl;
#endif
		// Board benutzt fixe Adressierung, z.B. DCC Board
		if	( board )
		{
			// GA Geraete vorhanden
			if	( buf.values[2] > 0 && buf.values[3] > 0 )
				DeviceManager.addAccessoire( new I2CGAMaster( buf.values[2], buf.values[3], i ) );

			// GL Geraete vorhanden
			if	( buf.values[4] > 0 && buf.values[5] > 0 )
				DeviceManager.addLoco( new I2CGLMaster( buf.values[4], buf.values[5], i ) );
		}
		// Standard I2C Board schaltet die Adresse pro Board um reservedXX weiter
		else
		{
			// FB Geraete vorhanden
			if	( buf.values[0] > 0 && buf.values[1] > 0 )
			{
				DeviceManager.addFeedback( new I2CFBMaster( startFB + buf.values[0], startFB + buf.values[1], i) );
				startFB += reservedFB;
			}
			// GA Geraete vorhanden
			if	( buf.values[2] > 0 && buf.values[3] > 0 )
			{
				DeviceManager.addAccessoire( new I2CGAMaster( startGA + buf.values[2], startGA + buf.values[3], i ) );
				startGA += reservedGA;
			}
			// GL (analoge) Geraete vorhanden
			if	( buf.values[4] > 0 && buf.values[5] > 0 )
				DeviceManager.addLoco( new I2CGLMaster( buf.values[4], buf.values[5], i ) );
		}
	}
}


} /* namespace i2c */
