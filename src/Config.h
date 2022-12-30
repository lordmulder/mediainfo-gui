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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor, 44)
MIXP_DEFINE_CONF(unsigned int, mixp_versionPatch,  0)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMajor, 22)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerMinor, 12)
MIXP_DEFINE_CONF(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_i686, "697f73b3707e7d61d0cf3cdf71aaa25fba60ee133191e72459114434efda4ff7868d7f70c83685d56a2b84744b32feba01a45a198ee9712aa9c6f3428febe992")
MIXP_DEFINE_CONF(char*, mixp_checksum_x86_sse2, "5423bfe4dc78b23601a308243b72f9f614fa374fb149ec6c437f9ecc203e705effccd8ba9aee124306444d0fb67015a45c3e1e6e37726de1d0c273747bbf18c3")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_sse2, "c9b6d9e5f6221fd5d12098a9e3e4f25ded9a451b67b7d3131c21a12785710c790901d2ce683c3a939657e742e72edc928c239472dc892dc73ff7d23d08997053")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64_avx2, "275c9514f40b2391d576304a035e928405ac162e6271b258cd028cb5d36d4f2a21d77144659637e86ee5df439e028526b0d346b50d7a8e41fadef80acc507bce")

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
