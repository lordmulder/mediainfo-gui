///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2019 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  34)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 19)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  4)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "820665985536fbe79dc88ea586d61fcd6d189e6c7444f18339f6515b99ff2e5264b59671237f35d4350e631729abf7fd130563110b449a6d240f342ca6cbf85b")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "8b9cf43c701be3f02241cd8062d484ae6b6fc05aa97725d6a958da7594d7d45bae479a05cb19f5a66ca3d4a6c09ddcc3c94a784991a0ae156206aafc4f725612")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "8819b142a6cb54862576cb1c2af8fc1b117479b57786ef6c8c293f6ba80479910618c2175c4b021dc74ab1e5a338ad7f9c6f19ab985b1ac3b28f10ff1579883a")

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
