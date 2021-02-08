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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMajor,  2)
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 38)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 20)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  9)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "fee397f96150db0028416cd8947b9e42a79eca26feeba235955d9abe9aa369164b34435f44858cf46b27c17688e7cdf7dcfbeb397b3701793e64ca5206d3ab79")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "1249935e049ab6a506199f21c95f14f0f7d28fedc8a5a89787de1e83d1c942003f8b483c348e1e294386cffdeb21eed473ceebf7b3c7d3595fa9e7736d131a8d")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "bbb8a24a97e3690ba19c65e92f5396fafdebb808937a21a6589a9e8fa47147ebd1de2629451bd3619bd0a387e156a86f02d76386a21b46904722032c97f0bd2f")

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
