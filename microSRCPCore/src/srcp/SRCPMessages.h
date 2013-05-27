/*
	SRCPMessages - SRCP Meldungen

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

#ifndef SRCPMESSAGES_H_
#define SRCPMESSAGES_H_

namespace srcp
{
class SRCPMessages
{
private:
	char buf[64];
public:
	char* error(int id);
	char* ok();
	char* ok201();
	char* ok202();
	char* go(int clientId);
	char* info( int port, char* device, int addr, int value);
	char* info( int bus, int addr, int cv, int value );
	const char *version ();
};

extern SRCPMessages Messages;
}

#endif /* SRCPMESSAGES_H_ */
