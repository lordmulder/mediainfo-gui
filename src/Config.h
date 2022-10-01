///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2022 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMajor,  2)
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 42)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 22)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  6)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "698cf0dbe2b6fcf2b56d4c77e041b4f5dbb4fc39f20d2ae7742dfde02c7669c1051034c2643aa99886d64f4140d28ace73aa7716b4f27b0a787f8774f452638d")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "1878d907ec130e0a5962a6c11ddd2ca7431bc3d13ad70475fe2c22376608199d9d029f8f52772970897dd7119d9eeca19a277195dac6fb83e3fa0b8809786e04")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "a61ac0daccba8dd1795906d9ec459fb42e0b242d98302bc2a77767f4eabf5c67badcda1fd11e790db4fe35f0d33445e2c4f6ef0dc702f59ad423640841ddfb03")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "22482f01075025171d841592caf52ec6109b600f6e9d635201f0b25b9c3ee03dec6ebab7544b4d5ea3bc3ef84f367f791b92a5702fb6e2797f8a451c8c65194a")

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
