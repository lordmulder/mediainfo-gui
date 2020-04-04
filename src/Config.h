///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2020 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  35)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 19)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  7)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "50f93f6f54d96751992c491755a037b1cb587d94a4fe673db77d86817f56adf2a3b9112edf6a37df428fb9162f5c5b565313921f9752a1579fed389da4d8a9d1")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "7addda6d9ef311bb5ed519c61420271aab1d495fb23f5bd44453bc48eb7a06b679b1b164cc0f15330a12ba3fce15332ffd80a6c11569080f6832a489821baa6d")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "46c5199a5b26cafc3340cb712a87b77314a0e234177d449dffcf1a8419a2639163091f7c43e93fe42966c748ef6ec24c3437f3574b0c977e2164fdb33b432064")

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
