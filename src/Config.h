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
MIXP_DEFINE_CONF(unsigned int, mixp_versionMinor,  26)

//MediaInfo Version
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMajor,  0)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionMinor,  7)
MIXP_DEFINE_CONF(unsigned int, mixp_miVersionPatch, 95)

//MediaInfo Checksum
MIXP_DEFINE_CONF(char*, mixp_checksum_gen, "3a75d87dc49d000f8d289d412b90b5e7be10fabbdf692e7cfe0fad1ad83ed67164022873760ceb67821479673f6e62339b89f58155a50833f343022218c27bc0")
MIXP_DEFINE_CONF(char*, mixp_checksum_sse, "2bdf393c77ee209c16fe34fdec674b4464f7ea8d1c5e74dbd8715c359316d59506756b66909b3c2aa7ec2fc52ad27a8825cd57dbd957b1401d0e2427480f4fb6")
MIXP_DEFINE_CONF(char*, mixp_checksum_x64, "52e8a60f949fa785b44163c6b52944f168f884b2226df0a2f7b6592699185152cba8161b3b68030b7f64e272b891ce12bac0ec38bd72eb409fd247f56b001341")

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
