/*
	SRCPEthernetServer - SRCP I/O ueber Ethernet

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

#ifndef SRCPETHERNETSERVER_H_
#define SRCPETHERNETSERVER_H_

#include <Arduino.h>
#include "../lan/EthernetSocket.h"
#include "SRCPCommand.h"
#include "SRCPMessages.h"
#include "SRCPParser.h"
#include "SRCPSession.h"

namespace srcp
{

class SRCPEthernetServer
{
private:
	lan::EthernetSocket* commandSocket;
	lan::EthernetSocket* infoSocket;
	srcp::SRCPSession* commandSession;
	srcp::SRCPSession* infoSession;
	srcp::command_t cmd;
	// ASCII SRCP Parser
	SRCPParser* parser;
	int status;
	int dispatch( srcp::SRCPSession* session, lan::EthernetSocket* socket );
public:
	void begin( byte* mac, IPAddress ip, unsigned int port );
	int dispatch();
};

} /* namespace srcp */

#endif /* SRCPETHERNETSERVER_H_ */
