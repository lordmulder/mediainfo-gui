///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2024 LoRd_MuldeR <MuldeR2@GMX.de>
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

#pragma once

#define MIXP_CONF_DECLARE(X,Y) extern const X g_##Y;

//Version
MIXP_CONF_DECLARE(unsigned int, mixp_versionMajor)
MIXP_CONF_DECLARE(unsigned int, mixp_versionMinor)
MIXP_CONF_DECLARE(unsigned int, mixp_versionPatch)

//MediaInfo Version
MIXP_CONF_DECLARE(unsigned int, mixp_mediaInfoVerMajor)
MIXP_CONF_DECLARE(unsigned int, mixp_mediaInfoVerMinor)
MIXP_CONF_DECLARE(unsigned int, mixp_mediaInfoVerPatch)

//MediaInfo Checksum
MIXP_CONF_DECLARE(char*, mixp_checksum_x86_i686)
MIXP_CONF_DECLARE(char*, mixp_checksum_x86_sse2)
MIXP_CONF_DECLARE(char*, mixp_checksum_x64_sse2)
MIXP_CONF_DECLARE(char*, mixp_checksum_x64_avx2)

//Build date
MIXP_CONF_DECLARE(char*, mixp_buildDate)
MIXP_CONF_DECLARE(char*, mixp_buildTime)

//Show console
#define MIXP_CONSOLE (0)

//Debug build
#if defined(_DEBUG) && defined(QT_DEBUG) && !defined(NDEBUG) && !defined(QT_NO_DEBUG)
	#define MIXP_DEBUG (1)
#else
	#define MIXP_DEBUG (0)
#endif

//Undefine
#undef MIXP_CONF_DECLARE
