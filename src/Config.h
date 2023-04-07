///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2023 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 45)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 23)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor, 03)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "c4d1cc08ea0a12a3a22bf1ebc0715681624a2313673b915f9b55a746b23e2d955fd2cea092debcfd5642a8d251365080ab5f79a1a0a166ffbd03fc07710a4edd")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "889b289f900653c2e64e632b4054e6c2ed19e1d4969ea17925147294aa642675485083e15c8405dcaf6b03c9e11618b8e9a2335cdd49fbfc16460597a1c763e0")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "9bc343ed15812122ab88a4ff123598f2fc4128c570f1e3ff9d05a8e9710ec2e6b41277522c1720a03e13aa556a249e12f8bba4e196b2cf9fe4a8bb4e93abe3fb")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "6f4c2fc6f0017355365dc24aee486ea8e5c9f05cc0b742f4e88808c193b7090c1775aa42bd87bf2ea9507540867cdd606ad3f1f8bc1d55134604c9a7e25e118e")

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
