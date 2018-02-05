///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2017 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  29)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVeMajor, 17)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVeMinor, 12)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "37554d094a19882260510dfead76bde6d42a26083fc0bd253706dda02d8c0d3b4205cb4fb4fe47852877ad01843eb4c03d133fda145b7dd7708488dd09207a01")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "130e08e4a69e0e7822fd48e47781eee4f9960496ad97af5f50bec30af7b607d5aade62074ba31b0f284a16c99176e825c6b959ae83488002198de46ed7ef9c4c")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "6c7783c0ed75dc8d6cd56943b8cc935b0b23970516ae523f1eb0d6d3fd259dc4ed408bbbf71cde184a1e6d7644c2770fa7235a9757976d578d47ac0bdf1fed0e")

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
