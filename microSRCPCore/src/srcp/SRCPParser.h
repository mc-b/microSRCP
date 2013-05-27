/*
	SRCPParser - Parst ASCII SRCP Messages

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

#ifndef SRCPPARSER_H_
#define SRCPPARSER_H_

#include "SRCPFeedback.h"
#include "SRCPGenericAccessoire.h"
#include "SRCPGenericLoco.h"
#include "SRCPCommand.h"

namespace srcp
{

class SRCPParser
{
private:
	devices getDevice( char* device );
public:
	void parse( char* args, int length = 0 );
};

} /* namespace srcp */
#endif /* SRCPPARSER_H_ */
