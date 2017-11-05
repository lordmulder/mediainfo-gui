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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  28)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVeMajor, 17)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVeMinor, 10)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "4c8966ae7ce9d4253543a8e818b01433cd149bb8f95c4a7b9a38dbc1c73f1f9cc45ea0416c10970c3b9daaa16ae1eca66a27fd7cec8bc5c5850f1f867a77098a")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "d5f6dd2584e75c24634ea776fd8d2dc832e945cd1540da82cdd5c10fa9056be602db479324ededdcdee09f81adb2303a4525f9b023ac4a5a1e6a0dfa63aed653")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "585162315280540b8faff9f34dc610ad3a25c8ac9415656b02454cabb7c1e5593fcc2309df2dd485c7cd99393b30ca846fe59dedeed951ee5340a106fcc970e2")

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
