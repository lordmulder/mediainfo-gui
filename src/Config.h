///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2021 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 39)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 21)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  3)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "2ebbd2b1fa84c8e48b49f8b504dc5c08a14e8757f4d17d5f60112ab4ebfcd061a2fdb609b0fb9059febc1088fe020f8ddf253f1bc18927a83d766eb217e72568")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "822149b4f9d2879bbd37c69b35c92c5bc4ed5c3dae9153dc30cee8fe3ae10d7116ade8c038885adae880f9b7bae0476aba17b5d60f9d8dcdcbe14e6d11cdd2b9")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "841bfe999635fde807b8679b89a9f1f03155112d358e16fa11ebda8d79813bd304745df7201f727554e6eaeb34b337e991c12725520f436d0e772a40c3a5a003")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "8f46d5be7cbd1a7bb9750a07d9891a725468506256dfeb0855fdeb8caead47295c977eff4891b9fc22aaeebcb81f1c216fec582b6b55598d0f67f4fd19b07e83")

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
