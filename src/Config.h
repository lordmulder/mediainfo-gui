///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2016 LoRd_MuldeR <MuldeR2@GMX.de>
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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  23)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMajor,  0)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMinor,  7)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionPatch, 91)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "9c767e75f9665e275a6ba656049b374ff2e41bf48dfed27e8de782f4040e532f2c06c3cba7727e5e2878641948b33952ccfcad7e5bf0cac7d6395c149692bc49")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "d7b355c992d6e48042180b383fb1cbcbff26375beacffcc3c1e58db1f8a450238f5d1535a511fd52c3453a8ac8d3d6b541685ea2d304fcfce10c96af48c6f944")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "af20eee9913060a702b772715dce522cc9bf96284689edd748ff007515270bb38a4d4078456b277b994b839ce07a14796b285743aa0833b82e4926453c5f64f9")

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
