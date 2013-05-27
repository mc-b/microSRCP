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

#ifndef FBSWITCHSENSOR_H_
#define FBSWITCHSENSOR_H_

#include <Arduino.h>
#include "../srcp/SRCPFeedback.h"

/**
 * Pins sind per default HIGH, deshalb werden sie mit Masse verbunden.
 */
namespace dev
{

class FBSwitchSensor : public srcp::SRCPFeedback
{
private:
	uint8_t startPin;
	uint8_t endPin;
public:
	FBSwitchSensor( int addr, uint8_t startPin, uint8_t endPin );
	void refresh();
};

}

#endif /* FBSWITCHSENSOR_H_ */
