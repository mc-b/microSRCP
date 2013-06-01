/*
	SRCPServerSerial - SRCP Server welche Meldungen mittels
	der Seriellen Schnittstellen empfaengt und sendet.

	Siehe auch: http://srcpd.sourceforge.net/srcp/

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

#ifndef SRCPSERVERSERIAL_H_
#define SRCPSERVERSERIAL_H_

#include <Arduino.h>
#include "SRCPSession.h"
#include "SRCPParser.h"

namespace srcp
{

class SRCPServerSerial
{
private:
	SRCPSession* session;
	SRCPParser* parser;
	srcp::command_t cmd;
public:
    void begin(unsigned long speed );
    command_t* dispatch(void);
    // sendet direkt an die COMMAND Session
    void sendCommand( char* message ) { Serial.println( message ); }
    // sendet direkt an die INFO Session
    void sendInfo( char* message )  { Serial.println( message ); }
};

} /* namespace srcp */
#endif /* SRCPSERVERSERIAL_H_ */
