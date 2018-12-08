///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2018 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  32)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 18)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  8)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  1)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "fefa7da0144edc0e20ef1949aabcd4b9a851ffe1e5103f0c4ec5dca85bf12d7123e9e59bf21cf24545b13db8ffb36e8098de316c3c9206461801fd1d8a1d4ff6")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "1bf147012b4ea77b4915d8024b6cce5e7fedff6f9dbbaeccc5857eb7672734f890e82d42acd5d16b339ecbd469d996a2d5ce4b0613941b5489d5a7fe53e9f6b9")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "bfbdaf7ac597c7a5427e3dd2445503eda18cf56996df82d2c85ab7e03429dd11d12702e4459332d4b5c6c1afa75bc4a37cf499da4ba885abae3c690056c238d4")

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
