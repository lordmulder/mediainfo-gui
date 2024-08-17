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
MIXP_CONF_DEFINE(char*, mixp_checksum_x86_i686, "ccfabcf0b55819a34d2ae33b4a3e578523c99314cc3a36909d5d145de3c3c6deb3e1d225bb2ed147566305fd194c1107b8588c6d5314f88efeb11c6ceefc987b")
MIXP_CONF_DEFINE(char*, mixp_checksum_x86_sse2, "a20c1ce741a3920dfafcb50009bc29bcb5850669d11e691a7aca244ef44def4f4563a4da4db3137c02467b204eeb72e89587591e78a95616a832e67c50d2cd11")
MIXP_CONF_DEFINE(char*, mixp_checksum_x64_sse2, "a2ec2e4268c8f5a95115149e552e06dd1ca7370ab8d252ed00b76b336698c97e454c32ecca62e0976b96306de435a074fa88cdfcd75bc9e97bf9c78ce99408f8")
MIXP_CONF_DEFINE(char*, mixp_checksum_x64_avx2, "e8dd8823fb95774cb240d21566d913017914ebacdb117931f5a41492e2530ae5622f1009eac4a0291400ef2e453c4d05e883f6dbef5461981be21246e815a3e5")

//Build date
MIXP_CONF_DEFINE(char*, mixp_buildDate, __DATE__)
MIXP_CONF_DEFINE(char*, mixp_buildTime, __TIME__)
