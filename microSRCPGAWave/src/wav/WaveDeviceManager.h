/*
	WaveDeviceManager - Erzeugt die Wave Geraete und stellt
	allgemeine Funktionen zum abspielen von Sounds zur
	Verfuegung.

	Copyright (c) 2010 - 2013 Marcel Bernet.  All right reserved.

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

#ifndef WAVEDEVICEMANAGER_H_
#define WAVEDEVICEMANAGER_H_

#include "WaveHC.h"

namespace wav
{

class WaveDeviceManager
{
private:
	static char* nextSound;
	static void playcomplete(char *name);
	static int playfile(char *name);
	static int ls();
public:
	static int init();
	static void play();
	static void setSound( char* next ) { nextSound = next; }
};

}

extern wav::WaveDeviceManager WaveDevManager;

#endif /* WAVEDEVICEMANAGER_H_ */
