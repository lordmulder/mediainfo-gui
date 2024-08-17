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

//Internal
#include "Config.h"
#include "Version.h"

#define MIXP_CONF_DEFINE(X,Y,Z) extern const X g_##Y = Z;

#define MIXP_VERSION_GLUE_HELPER(X,Y) X##Y
#define MIXP_VERSION_GLUE(X,Y) MIXP_VERSION_GLUE_HELPER(X,Y)
#define MIXP_VERSION_MINOR MIXP_VERSION_GLUE(MIXP_VERSION_MINOR_HI,MIXP_VERSION_MINOR_LO)

//Version
MIXP_CONF_DEFINE(unsigned int, mixp_versionMajor, MIXP_VERSION_MAJOR)
MIXP_CONF_DEFINE(unsigned int, mixp_versionMinor, MIXP_VERSION_MINOR)
MIXP_CONF_DEFINE(unsigned int, mixp_versionPatch, MIXP_VERSION_PATCH)

//MediaInfo Version
MIXP_CONF_DEFINE(unsigned int, mixp_mediaInfoVerMajor, 23)
MIXP_CONF_DEFINE(unsigned int, mixp_mediaInfoVerMinor, 11)
MIXP_CONF_DEFINE(unsigned int, mixp_mediaInfoVerPatch,  0)

//MediaInfo Checksum
MIXP_CONF_DEFINE(char*, mixp_checksum_x86_i686, "39620b62dede4834f93bfa0ef6d39473c2d8b0632e176ce42f25bd6e5cfed32867a3c47026a05c7610af3da9839044dd1740534636b28aaf8d8966f8384ce3b9")
MIXP_CONF_DEFINE(char*, mixp_checksum_x86_sse2, "bc3283aeb07255d5465594fe149d61af4b07dbca85a844454a06340dcc3292eeef502a3641c3e299209144fa60a180fed40c79d4dc904ec8f6349d2f4140de95")
MIXP_CONF_DEFINE(char*, mixp_checksum_x64_sse2, "69d4208d7125505e35472773facfb64b138f7d9595e4c2338dda9f420697661c8271e7ef449eeeeaf8134b06fab9452760887577fbb2b387af49e44fe928fc98")
MIXP_CONF_DEFINE(char*, mixp_checksum_x64_avx2, "75c0fd7d3fe9223a9ccf26c74ccae144c62b29c3e2ac0023655b22cc0f2196e22af9d27c005fb7c29573e527c36f0126faea666e0b995bf6e10a019e6aabc61f")

//Build date
MIXP_CONF_DEFINE(char*, mixp_buildDate, __DATE__)
MIXP_CONF_DEFINE(char*, mixp_buildTime, __TIME__)
