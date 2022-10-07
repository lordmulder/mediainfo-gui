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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 43)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 22)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  9)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "7fc1383f1d74d7d9ed8f85aa0cd96e200e143e16d1b7e21bb727b58a7e92b5edaf19d0b81fbedf3a2577264f92ea7e7788aaf9253423f37339eecdde7512f4cd")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "d415f676f568967dfdc1f1ff3b9ffc45a73d223564307933b5c3ee7f27cfc75639d923324723d171f33edc7e8fd9dd36157b4b605f1b50d558cb93fb785c853a")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "248a89de21861a9fa4a7181e083adffbb6e458a18dd5e789128d753b3f0ce6aca9648ae8deb164a979e93ec3e66ad111500eaadfa0df337093167df048cfa856")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "904815fa8507a3acfb666f0452bb7c620ba2132155eb35a57fcafffed55d665374f24289bbe77788d10f91588407f604b2ae25fcd62a53994c385914c246f569")

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
