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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  25)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMajor,  0)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMinor,  7)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionPatch, 94)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "4e52dbccaefd0a0e2b28a1e5a865a995097f195fee7bf290fa2b3f0d8bc557e90da4d0c28191a4b48c17d4f61f5f90b7846289daeaf4e85e3a13db6afead6ae2")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "4310cb71abb7c1611f3c60af0f52c5e807a783a27cb67291059696fbe04bd2710522b713b1e8f5d46d575819e6c5b4075ee50fbb729d06efeb201c77db112b1e")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "fd0ab34aaa112a39e69372443ce1cc9813c7edac2691551971d46d96d3f146d221a5db532f210c8fb8d26095b0d09f6b8c206ba19e92a3b3e3c1c9dc38b07877")

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
