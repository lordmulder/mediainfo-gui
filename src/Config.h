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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 41)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 22)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  3)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "248e9e0211290e317e4384fe45043a42ecba20cf6b02c0216f821cf7d2515623d931beabb6a882a2c3ad6e1d08bec65015bf7bf00630b72f5e117ca0a4755256")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "03fd624a566e13b70d35b8b109da8e0378faeab44c7de8a221128dcdddcdbd1e446a2e073a15cff9a667264ba88a928b74c3e1b2c33153530f066337b6f6246f")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "84b0d58dfcd93e4d0dcf85e2b3a91e80d5194dd0a680996a4c625509e0befe0499b5a41557e594d32dcfdb54ebac6430b443c6dd29b95133ae9d50f9e684f43e")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "8261d2ed869bc434e3b20b6a75d457ab677e9fcc93652c09853fc0993703e47cd3e9713084816cf1d6de3800ba1a0cda5679983b347ca42fccb4c163fdadea7d")

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
