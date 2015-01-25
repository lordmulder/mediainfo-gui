///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2015 LoRd_MuldeR <MuldeR2@GMX.de>
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

#include "ShellExtension.h"
#include "Utils.h"

#include <QApplication>
#include <QDir>

#define MIXP_REGISTRY_KEY "Software\\Classes\\*\\shell\\MediaInfoXP"
#define MIXP_REGISTRY_VAL "_shellExtEnabled"

bool ShellExtension::getEnabled(void)
{
	quint32 value = 0;
	if(mixp_reg_value_read(mixp_root_user, MIXP_REGISTRY_KEY, MIXP_REGISTRY_VAL, value))
	{
		return value;
	}
	return false;
}

bool ShellExtension::setEnabled(bool enabled)
{
	if(enabled)
	{
		qDebug("Installing the shell extension...");
		if(mixp_reg_value_write(mixp_root_user, MIXP_REGISTRY_KEY, QString(), tr("Analyze file with MediaInfoXP")))
		{
			const QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());
			const QString command = QString().sprintf("\"%ls\" --open \"%%1\"", appPath.utf16());
			if(mixp_reg_value_write(mixp_root_user, MIXP_REGISTRY_KEY"\\command", QString(), command))
			{
				if(mixp_reg_value_write(mixp_root_user, MIXP_REGISTRY_KEY, MIXP_REGISTRY_VAL, 1))
				{
					qDebug("Success.\n");
					mixp_shell_change_notification();
					return true;
				}
			}
		}
		qWarning("Failed to install the shell extension!\n");
		mixp_reg_key_delete(mixp_root_user, MIXP_REGISTRY_KEY);
		return false;
	}
	else
	{
		qDebug("Un-installing the shell extension...");
		if(!mixp_reg_key_delete(mixp_root_user, MIXP_REGISTRY_KEY))
		{
			qWarning("Failed to un-install the shell extension!\n");
			return false;
		}
		qDebug("Success.\n");
		mixp_shell_change_notification();
		return true;
	}
}
