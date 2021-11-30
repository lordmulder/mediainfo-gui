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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 40)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 21)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor,  9)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "e07f44f17d485020820a9fa71bef96f66c1299498a2ff4b95a53664487d04f5fc0bce3bdd645dfcc22be78aa39b2153fbf68723f223f4b874f490778d34a87a8")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "9d17acd9acf4c93ec8a61e3597367ead962b81b31659d499be7449eb146ddb7c721858b3a1a84803c307d8cf575d0526cab29944a6bff9223b9907a4a7e0701b")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "5e4f715669de6d1cd346175a43602e82f46fb396b7548c1d127be43776292f6bd43e4a2ec685193933f174880927092d9de22c005d0b1f4fb0668129eff86ac3")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "f4831a53f777fd1e3951aa27e32213d51d2cbfbba02cd34f698b74e968dc4eacceb8345e42782d9e77b381d988af1079a20e5065a9ef4c798a3066f4eed39f64")

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
