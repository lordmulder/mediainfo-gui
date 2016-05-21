///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2016 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// http://www.gnu.org/licenses/gpl-2.0.txt
///////////////////////////////////////////////////////////////////////////////

#ifdef MIXP_CREATE_CONFIG
#define MIXP_DEFINE_CONF(X,Y,Z) extern const X g_##Y = Z;
#else
#define MIXP_DEFINE_CONF(X,Y,Z) extern const X g_##Y;
#endif

//Version
MIXP_DEFINE_CONF(unsigned int, mixp_versionMajor,   2)
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  20)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMajor,  0)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMinor,  7)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionPatch, 85)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86, "b5a8498b6e1f1c9c936d17e5ced250b6aceca92010f224eccef8e211207f205b056c9b9982c2ed3b176c6b723a10f91c24803f9087cc75c829a97b3adb27d58b")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "1bce101bc88b874de57da929657a17a43785da777a0a77c95e8b5ec24c9c1f25db58ec1e53bcb9476c537f455c7eafeb0ac61b3b8d1d271157afb2633c54764f")

//Build date
MIXP_DEFINE_CONF(char*, mixp_buildDate, __DATE__)
MIXP_DEFINE_CONF(char*, mixp_buildTime, __TIME__)

//Show console
#define MIXP_CONSOLE (0)

//Debug build
#if defined(_DEBUG) && defined(QT_DEBUG) && !defined(NDEBUG) && !defined(QT_NO_DEBUG)
	#define MIXP_DEBUG (1)
#else
	#define MIXP_DEBUG (0)
#endif

//Undefine
#undef MIXP_DEFINE_CONF
