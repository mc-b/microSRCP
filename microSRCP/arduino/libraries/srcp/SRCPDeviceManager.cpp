/*
	SRCPDeviceManager leitet die SRCP Befehle an die Geraete weiter

	Siehe auch: http://srcpd.sourceforge.net/srcp/

	Copyright (c) 2012 Marcel Bernet.  All right reserved.

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

#include "SRCPDeviceManager.h"

#include "SRCPGenericAccessoire.h"
#include "SRCPDevice.h"
#if	( DEBUG_SCOPE > 1 )
#include <HardwareSerial.h>
#include <Streaming.h>
#endif

namespace srcp
{

SRCPDeviceManager::SRCPDeviceManager()
{
	firstGA = (srcp::SRCPGenericAccessoire*) 0;
	firstGL = (srcp::SRCPGenericLoco*) 0;
	firstFB = (srcp::SRCPFeedback*) 0;
}

int SRCPDeviceManager::setGA( int addr, int port, int value, int delay )
{
	SRCPGenericAccessoire* next = (srcp::SRCPGenericAccessoire*) 0;
	int rc = 200;

#if	( DEBUG_SCOPE > 1 )
	Serial3 << "set" + addr << " " << (int) firstGA << endl;
#endif

	for	( next = firstGA; next != (srcp::SRCPGenericAccessoire*) 0; next = next->nextElement() )
	{
		if	( next->checkAddr( addr ) == 0 )
			continue;
		rc = next->set( addr, port, value, delay );
	}
	return	( rc );

}
int SRCPDeviceManager::setGL( int addr, int drivemode, int v, int v_max, int fn[] )
{
	SRCPGenericLoco* next = (srcp::SRCPGenericLoco*) 0;
	int rc = 200;

	for	( next = firstGL; next != (srcp::SRCPGenericLoco*) 0; next = next->nextElement() )
	{
		if	( next->checkAddr( addr ) == 0 )
			continue;
		rc = next->set( addr, drivemode, v, v_max, fn );
	}
	return	( rc );
}

void SRCPDeviceManager::setPower( int on )
{
	SRCPGenericAccessoire* ga = (srcp::SRCPGenericAccessoire*) 0;
	for	( ga = firstGA; ga != (srcp::SRCPGenericAccessoire*) 0; ga = ga->nextElement() )
		ga->setPower( on );

	SRCPGenericLoco* gl = (srcp::SRCPGenericLoco*) 0;
	for	( gl = firstGL; gl != (srcp::SRCPGenericLoco*) 0; gl = gl->nextElement() )
		gl->setPower( on );
}

void SRCPDeviceManager::refresh()
{
	for	( SRCPFeedback* next = firstFB; next != (srcp::SRCPFeedback*) 0; next = next->nextElement() )
			next->refresh();
}

} /* namespace srcp */

/** Globaler Device Manager */
srcp::SRCPDeviceManager DeviceManager = srcp::SRCPDeviceManager();

