/*
	SRCPSession - Handelt eine SRCP Session ab, z.B.
	zusammen mit EthernetSRCPServer verwenden.

	Das Protokoll entspricht mindestens 0.8.2
	Siehe auch: http://srcpd.sourceforge.net/srcp/

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

#include <stdio.h>
#include <Streaming.h>
#include "SRCPSession.h"
#include "SRCPMessages.h"

namespace srcp
{

/** Globaler Counter wird bei jedem SRCP GO um ein aufgezaehlt */
int SRCPSession::counter = 1;

// Globaler IO-Buffer fuer Sensoren - Platz fuer 8 Sensoren und 9 als Endmerker \0
feedback fb[9];

SRCPSession::SRCPSession()
{
	status = UNDEFINED;
	power = OFF;
}

char* SRCPSession::dispatch()
{
	switch (global_cmd.cmd)
	{
		case GO:
			power = ON;
			return	( Messages.go( counter++ ));
		case GET:
			switch (global_cmd.device)
			{
				case SM:
				{
					int rc = DeviceManager.getSM( global_cmd.bus, global_cmd.addr, global_cmd.values[0], global_cmd.values[1] );
					if	( rc == -1 )
						return	( Messages.error( 421 ) );
					return	( Messages.info( global_cmd.bus, global_cmd.addr, global_cmd.values[0], rc ));
				}
				case POWER:
					return	( Messages.info( 0, "POWER", 0, power ));
			}
			return (Messages.ok());
		case SET:
			switch (global_cmd.device)
			{
				case POWER:
					power = (power_enum) global_cmd.values[0];
					DeviceManager.setPower( power );
					return (Messages.ok());

				case FB:
					return (Messages.ok());

				case GA:
					global_cmd.values[0] = DeviceManager.setGA( global_cmd.addr, global_cmd.values[0], global_cmd.values[1], global_cmd.values[2] );
					return (Messages.ok());

				case GL:
					global_cmd.values[0] = DeviceManager.setGL( global_cmd.addr, global_cmd.values[0], global_cmd.values[1], global_cmd.values[2], global_cmd.values );
					return (Messages.ok());

				case SM:
					DeviceManager.setSM( global_cmd.bus, global_cmd.addr, global_cmd.values[0], global_cmd.values[1], global_cmd.values[2] );
					return (Messages.ok());

				default:
					return (Messages.error(421));
			}
			break;
		case CONNECTIONMODE:
			status = global_cmd.values[0];
			return	( Messages.ok202() );

		case PROTOCOL:
			return	( Messages.ok201() );

		default:
			return (Messages.ok());
	}
	//return (Messages.error(500));
	return (Messages.ok());
}

const char* SRCPSession::version()
{
	status = HANDSHAKE;
	return	( Messages.version() );
}

void SRCPSession::infoFeedback( Print* out )
{
	for	( SRCPFeedback* next = DeviceManager.firstFeedbackElement(); next != (srcp::SRCPFeedback*) 0; next = next->nextElement() )
	{
		next->info( 0, &fb[0] );
		for	( int i = 0; fb[i].pin != 0; i++ )
		{
			char* m = Messages.info( 0, "FB", fb[i].pin, fb[i].value );
#if	( DEBUG_SCOPE > 1 )
			Serial3 << "send : " << getStatus() << ", " << m << '\r';
#endif
			out->print( m );
		}
	}
}

void SRCPSession::disconnect()
{
	power = OFF;
	if	( status == COMMAND )
		DeviceManager.setPower( power );
	status = UNDEFINED;
};

}
