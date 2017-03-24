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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  24)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMajor,  0)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMinor,  7)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionPatch, 93)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "17a4ebd6acd1b72c22c5b8b4fbcdbec9343b3a0ba18b0897ad3839e647bc8c54ff9062e1ee53264d25e5394fe2e1b850fab785b5e68d1a1298aaa12a8f2a5d79")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "0f43c3fcc61124a0d6af83b940f2c8e43245e92bfc683fa265069b79e32dcb560f80927872683cf117b196f5fe5b7f09337b04c7cd7d9b8e204e07adf6967d13")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "0025a5e7155c4a8c5cd113e94f79b7d5c6e5fd3767e94b3413031dccf63c44725fd72c78197cf044ddeea555858b0ba875852bc488d89d51cafb810495e449c6")

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
