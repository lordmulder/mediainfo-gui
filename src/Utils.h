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

#include <QString>
#include <QFile>

class QDate;
class QWidget;
class QIcon;
class mixp_icon_t;

//Helper macros
#define MIXP_DELETE_OBJ(PTR) do { if((PTR)) { delete ((PTR)); (PTR) = NULL; } } while (0)
#define MIXP_DELETE_ARR(PTR) do { if((PTR)) { delete [] ((PTR)); (PTR) = NULL; } } while (0)
#define QWCHAR(STR) reinterpret_cast<const wchar_t*>(STR.utf16())

//Beep types
typedef enum
{
	mixp_beep_info = 0,
	mixp_beep_warning = 1,
	mixp_beep_error = 2
}
mixp_beep_t;

//Regsitry root
typedef enum
{
	mixp_root_classes = 0,
	mixp_root_user = 1,
	mixp_root_machine = 2,
}
mixp_reg_root_t;

//Utils
QString mixp_getTempFolder(QFile **lockfile);
bool mixp_clean_folder(const QString &folderPath);
QDate mixp_get_build_date(void);
QDate mixp_get_current_date(void);
mixp_icon_t *mixp_set_window_icon(QWidget *window, const QIcon &icon, const bool bIsBigIcon);
void mixp_free_window_icon(mixp_icon_t *icon);
bool mixp_beep(int beepType);
bool mixp_bring_to_front(const QWidget *window);
void mixp_shell_change_notification(void);

//Regsitry
bool mixp_reg_value_write(int rootKey, const QString &keyName, const QString &valueName, const quint32 value);
bool mixp_reg_value_write(int rootKey, const QString &keyName, const QString &valueName, const QString &value);
bool mixp_reg_value_read(int rootKey, const QString &keyName, const QString &valueName, quint32 &value);
bool mixp_reg_key_delete(int rootKey, const QString &keyName);

//Init
void _mixp_global_init(void);
