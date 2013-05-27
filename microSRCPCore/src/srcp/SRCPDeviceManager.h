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

#ifndef SRCPDEVICEMANAGER_H_
#define SRCPDEVICEMANAGER_H_

#include "SRCPCommand.h"
#include "SRCPGenericAccessoire.h"
#include "SRCPGenericLoco.h"
#include "SRCPFeedback.h"

namespace srcp
{

class SRCPDeviceManager
{
private:
	SRCPGenericAccessoire *firstGA;
	SRCPGenericLoco *firstGL;
	SRCPFeedback *firstFB;
public:
	SRCPDeviceManager();
	void addDevice( SRCPGenericAccessoire* device ) { firstGA->setNextElement(this->firstGA); this->firstGA = firstGA; }
	void addDevice( SRCPGenericLoco* device ) { firstGL->setNextElement(this->firstGL); this->firstGL = firstGL; }
	void addDevice( SRCPDevice* device ) { firstFB->setNextElement(this->firstFB); this->firstFB = firstFB; }

	int setGA( int addr, int port, int value, int delay );
	int setGL( int addr, int drivemode, int v, int v_max, int fn[] );

	void refresh();
	int getFB( int addr ) { return( 200 ); };
	void setPower( int on );
	int setSM( int bus, int addr, int device, int cv, int value ) { return( 200 ); };
	int getSM( int bus, int addr, int device, int cv ) { return( 200 ); };
	int getDescription( int bus, int addr, int device, int rc[] ) { return( 200 ); };
};
} /* namespace srcp */

extern srcp::SRCPDeviceManager DeviceManager;

#endif /* SRCPDEVICEMANAGER_H_ */
