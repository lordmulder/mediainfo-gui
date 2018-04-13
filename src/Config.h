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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  30)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 18)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor, 03)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch, 01)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "879e8dde33eade79bfd2dc0084636121146639749103241cb4ce5150a676b895106455d0fdceb37c53b7eb4d955e020960004324ca4e5aba24b8b39f2c0278f4")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "d8dff1d215075df2c1529f4f3777a370fe69a0b64e22f064faf3a66ee8b1b5356c167c52568fab8436a7b072ee4d69a09f164443d99c8ec691a85b6d11573219")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "8ae0287b2960ffb525ef6b2377f8c22d8c70d80ddc1d324e3373b38314e35688a0d2279db0868d94ec9e002df60a0a4290ff14a98eb70cac3dcf3acfbe28e674")

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
