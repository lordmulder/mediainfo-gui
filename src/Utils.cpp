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

#include "Utils.h"
#include "Config.h"

//Win32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <Objbase.h>
#include <Psapi.h>
#include <Shlobj.h>
#include <Shlwapi.h>

//StdLib
#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>

#pragma intrinsic(_InterlockedExchange)

//Qt
#include <QLibrary>
#include <QDir>
#include <QFileInfo>
#include <QUuid>
#include <QDate>
#include <QReadWriteLock>
#include <QMap>
#include <QIcon>
#include <QWidget>

//Function pointers
typedef HRESULT (WINAPI *SHGetKnownFolderPath_t)(const GUID &rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
typedef HRESULT (WINAPI *SHGetFolderPath_t)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);

//Known folders
typedef enum
{
	mixp_folder_localappdata = 0,
	mixp_folder_programfiles = 2,
	mixp_folder_systemfolder = 3,
	mixp_folder_systroot_dir = 4
}
mixp_known_folder_t;

//Known folder cache
static struct
{
	bool initialized;
	QMap<size_t, QString> knownFolders;
	SHGetKnownFolderPath_t getKnownFolderPath;
	SHGetFolderPath_t getFolderPath;
	QReadWriteLock lock;
}
g_mixp_known_folder;

/*
 * Try to lock folder
 */
static QString mixp_tryLockFolder(const QString &folderPath, QFile **lockfile)
{
	const QByteArray WRITE_TEST_DATA = QByteArray("Lorem ipsum dolor sit amet, consetetur sadipscing elitr!");

	for(int i = 0; i < 32; i++)
	{
		QDir folder(folderPath);
		if(!folder.exists())
		{
			folder.mkdir(".");
		}

		if(folder.exists())
		{
			const QString SUB_FOLDER = QUuid::createUuid().toString().remove('{').remove('}').remove('-').right(16);

			folder.mkdir(SUB_FOLDER);
			if(folder.cd(SUB_FOLDER) && folder.exists())
			{
				QFile *testFile = new QFile(QString("%1/~lock.tmp").arg(folder.canonicalPath()));
				if(testFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
				{
					if(testFile->write(WRITE_TEST_DATA) >= WRITE_TEST_DATA.size())
					{
						*lockfile = testFile;
						return folder.canonicalPath();
					}
					testFile->remove();
				}
				MIXP_DELETE_OBJ(testFile);
			}
		}
	}

	return QString();
}

/*
 * Locate known folder on local system
 */
static const QString &mixp_known_folder(mixp_known_folder_t folder_id)
{
	//static const int CSIDL_FLAG_CREATE = 0x8000;
	//typedef enum { KF_FLAG_CREATE = 0x00008000 } kf_flags_t;
	
	struct
	{
		const int csidl;
		const GUID guid;
	}
	static s_folders[] =
	{
		{ 0x001c, {0xF1B32785,0x6FBA,0x4FCF,{0x9D,0x55,0x7B,0x8E,0x7F,0x15,0x70,0x91}} },  //CSIDL_LOCAL_APPDATA
		{ 0x0026, {0x905e63b6,0xc1bf,0x494e,{0xb2,0x9c,0x65,0xb7,0x32,0xd3,0xd2,0x1a}} },  //CSIDL_PROGRAM_FILES
		{ 0x0024, {0xF38BF404,0x1D43,0x42F2,{0x93,0x05,0x67,0xDE,0x0B,0x28,0xFC,0x23}} },  //CSIDL_WINDOWS_FOLDER
		{ 0x0025, {0x1AC14E77,0x02E7,0x4E5D,{0xB7,0x44,0x2E,0xB1,0xAE,0x51,0x98,0xB7}} },  //CSIDL_SYSTEM_FOLDER
	};

	size_t folderId = size_t(-1);

	switch(folder_id)
	{
		case mixp_folder_localappdata: folderId = 0; break;
		case mixp_folder_programfiles: folderId = 1; break;
		case mixp_folder_systroot_dir: folderId = 2; break;
		case mixp_folder_systemfolder: folderId = 3; break;
	}

	if(folderId == size_t(-1))
	{
		qWarning("Invalid 'known' folder was requested!");
		return *reinterpret_cast<QString*>(NULL);
	}

	QReadLocker readLock(&g_mixp_known_folder.lock);

	//Already in cache?
	if(g_mixp_known_folder.knownFolders.contains(folderId))
	{
		return g_mixp_known_folder.knownFolders[folderId];
	}

	//Obtain write lock to initialize
	readLock.unlock();
	QWriteLocker writeLock(&g_mixp_known_folder.lock);

	//Still not in cache?
	if(g_mixp_known_folder.knownFolders.contains(folderId))
	{
		return g_mixp_known_folder.knownFolders[folderId];
	}

	//Initialize on first call
	if(!g_mixp_known_folder.initialized)
	{
		QLibrary shell32("shell32.dll");
		if(shell32.load())
		{
			g_mixp_known_folder.getFolderPath =      (SHGetFolderPath_t)      shell32.resolve("SHGetFolderPathW");
			g_mixp_known_folder.getKnownFolderPath = (SHGetKnownFolderPath_t) shell32.resolve("SHGetKnownFolderPath");
		}
		g_mixp_known_folder.initialized = true;
	}

	QString folderPath;

	//Now try to get the folder path!
	if(g_mixp_known_folder.getKnownFolderPath)
	{
		WCHAR *path = NULL;
		if(g_mixp_known_folder.getKnownFolderPath(s_folders[folderId].guid, KF_FLAG_CREATE, NULL, &path) == S_OK)
		{
			//MessageBoxW(0, path, L"SHGetKnownFolderPath", MB_TOPMOST);
			QDir folderTemp = QDir(QDir::fromNativeSeparators(QString::fromUtf16(reinterpret_cast<const unsigned short*>(path))));
			if(folderTemp.exists())
			{
				folderPath = folderTemp.canonicalPath();
			}
			CoTaskMemFree(path);
		}
	}
	else if(g_mixp_known_folder.getFolderPath)
	{
		WCHAR *path = new WCHAR[4096];
		if(g_mixp_known_folder.getFolderPath(NULL, s_folders[folderId].csidl | CSIDL_FLAG_CREATE, NULL, NULL, path) == S_OK)
		{
			//MessageBoxW(0, path, L"SHGetFolderPathW", MB_TOPMOST);
			QDir folderTemp = QDir(QDir::fromNativeSeparators(QString::fromUtf16(reinterpret_cast<const unsigned short*>(path))));
			if(folderTemp.exists())
			{
				folderPath = folderTemp.canonicalPath();
			}
		}
		MIXP_DELETE_ARR(path);
	}

	//Update cache
	g_mixp_known_folder.knownFolders.insert(folderId, folderPath);
	return g_mixp_known_folder.knownFolders[folderId];
}

/*
 * Detect the TEMP folder
 */
QString mixp_getTempFolder(QFile **lockfile)
{
	*lockfile = NULL;

	//Try the %TMP% or %TEMP% directory first
	QString tempPath = mixp_tryLockFolder(QDir::temp().absolutePath(), lockfile);
	if(!tempPath.isEmpty())
	{
		return tempPath;
	}

	qWarning("Failed to init %%TEMP%%, falling back to %%LOCALAPPDATA%% or %%SYSTEMROOT%%\n");

	//Create TEMP folder in %LOCALAPPDATA%
	for(int i = 0; i < 2; i++)
	{
		static const mixp_known_folder_t folderId[2] = { mixp_folder_localappdata, mixp_folder_systroot_dir };
		const QString &localAppDataPath = mixp_known_folder(folderId[i]);
		if(!localAppDataPath.isEmpty())
		{
			if(QDir(localAppDataPath).exists())
			{
				tempPath = mixp_tryLockFolder(QString("%1/Temp").arg(localAppDataPath), lockfile);
				if(!tempPath.isEmpty())
				{
					return tempPath;
				}
			}
		}
	}

	return QString();
}

/*
 * Safely remove a file
 */
static bool mixp_remove_file(const QString &filename)
{
	if(!QFileInfo(filename).exists() || !QFileInfo(filename).isFile())
	{
		return true;
	}
	else
	{
		if(!QFile::remove(filename))
		{
			static const DWORD attrMask = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
			const DWORD attributes = GetFileAttributesW(QWCHAR(filename));
			if(attributes & attrMask)
			{
				SetFileAttributesW(QWCHAR(filename), FILE_ATTRIBUTE_NORMAL);
			}
			if(!QFile::remove(filename))
			{
				qWarning("Could not delete \"%s\"", filename.toLatin1().constData());
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
}

/*
 * Clean folder
 */
bool mixp_clean_folder(const QString &folderPath)
{
	QDir tempFolder(folderPath);
	if(tempFolder.exists())
	{
		QFileInfoList entryList = tempFolder.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);

		for(int i = 0; i < entryList.count(); i++)
		{
			if(entryList.at(i).isDir())
			{
				mixp_clean_folder(entryList.at(i).canonicalFilePath());
			}
			else
			{
				for(int j = 0; j < 3; j++)
				{
					if(mixp_remove_file(entryList.at(i).canonicalFilePath()))
					{
						break;
					}
				}
			}
		}
		return tempFolder.rmdir(".");
	}
	return true;
}

/*
 * Get build date
 */
QDate mixp_get_build_date(void)
{
	QDate buildDate(2000, 1, 1);

	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	int date[3] = {0, 0, 0}; char temp[12] = {'\0'};
	strncpy_s(temp, 12, mixp_buildDate, _TRUNCATE);

	if(strlen(temp) == 11)
	{
		temp[3] = temp[6] = '\0';
		date[2] = atoi(&temp[4]);
		date[0] = atoi(&temp[7]);
			
		for(int j = 0; j < 12; j++)
		{
			if(!_strcmpi(&temp[0], months[j]))
			{
				date[1] = j+1;
				break;
			}
		}

		buildDate = QDate(date[0], date[1], date[2]);
	}

	return buildDate;
}

/*
 * Get current date
 */
QDate mixp_get_current_date(void)
{
	const DWORD MAX_PROC = 1024;
	DWORD *processes = new DWORD[MAX_PROC];
	DWORD bytesReturned = 0;
	
	if(!EnumProcesses(processes, sizeof(DWORD) * MAX_PROC, &bytesReturned))
	{
		MIXP_DELETE_ARR(processes);
		return QDate::currentDate();
	}

	const DWORD procCount = bytesReturned / sizeof(DWORD);
	ULARGE_INTEGER lastStartTime;
	memset(&lastStartTime, 0, sizeof(ULARGE_INTEGER));

	for(DWORD i = 0; i < procCount; i++)
	{
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processes[i]);
		if(hProc)
		{
			FILETIME processTime[4];
			if(GetProcessTimes(hProc, &processTime[0], &processTime[1], &processTime[2], &processTime[3]))
			{
				ULARGE_INTEGER timeCreation;
				timeCreation.LowPart = processTime[0].dwLowDateTime;
				timeCreation.HighPart = processTime[0].dwHighDateTime;
				if(timeCreation.QuadPart > lastStartTime.QuadPart)
				{
					lastStartTime.QuadPart = timeCreation.QuadPart;
				}
			}
			CloseHandle(hProc);
		}
	}

	MIXP_DELETE_ARR(processes);
	
	FILETIME lastStartTime_fileTime;
	lastStartTime_fileTime.dwHighDateTime = lastStartTime.HighPart;
	lastStartTime_fileTime.dwLowDateTime = lastStartTime.LowPart;

	FILETIME lastStartTime_localTime;
	if(!FileTimeToLocalFileTime(&lastStartTime_fileTime, &lastStartTime_localTime))
	{
		memcpy(&lastStartTime_localTime, &lastStartTime_fileTime, sizeof(FILETIME));
	}
	
	SYSTEMTIME lastStartTime_system;
	if(!FileTimeToSystemTime(&lastStartTime_localTime, &lastStartTime_system))
	{
		memset(&lastStartTime_system, 0, sizeof(SYSTEMTIME));
		lastStartTime_system.wYear = 1970; lastStartTime_system.wMonth = lastStartTime_system.wDay = 1;
	}

	const QDate currentDate = QDate::currentDate();
	const QDate processDate = QDate(lastStartTime_system.wYear, lastStartTime_system.wMonth, lastStartTime_system.wDay);
	return (currentDate >= processDate) ? currentDate : processDate;
}

/*
 * Convert QIcon to HICON -> caller is responsible for destroying the HICON!
 */
static HICON mixp_qicon2hicon(const QIcon &icon, const int w, const int h)
{
	if(!icon.isNull())
	{
		QPixmap pixmap = icon.pixmap(w, h);
		if(!pixmap.isNull())
		{
			return pixmap.toWinHICON();
		}
	}
	return NULL;
}

/*
 * Update the window icon
 */
mixp_icon_t *mixp_set_window_icon(QWidget *window, const QIcon &icon, const bool bIsBigIcon)
{
	if(!icon.isNull())
	{
		const int extend = (bIsBigIcon ? 32 : 16);
		if(HICON hIcon = mixp_qicon2hicon(icon, extend, extend))
		{
			SendMessage(window->winId(), WM_SETICON, (bIsBigIcon ? ICON_BIG : ICON_SMALL), LPARAM(hIcon));
			return reinterpret_cast<mixp_icon_t*>(hIcon);
		}
	}
	return NULL;
}

/*
 * Free window icon
 */
void mixp_free_window_icon(mixp_icon_t *icon)
{
	if(HICON hIcon = reinterpret_cast<HICON>(icon))
	{
		DestroyIcon(hIcon);
	}
}

/*
 * Message Beep
 */
bool mixp_beep(int beepType)
{
	switch(beepType)
	{
		case mixp_beep_info:    return (MessageBeep(MB_ICONASTERISK) != FALSE);    break;
		case mixp_beep_warning: return (MessageBeep(MB_ICONEXCLAMATION) != FALSE); break;
		case mixp_beep_error:   return (MessageBeep(MB_ICONHAND) != FALSE);        break;
		default: return false;
	}
}

/*
 * Bring the specifed window to the front
 */
bool mixp_bring_to_front(const QWidget *window)
{
	bool ret = false;
	
	if(window)
	{
		for(int i = 0; (i < 5) && (!ret); i++)
		{
			ret = (SetForegroundWindow(window->winId()) != FALSE);
			SwitchToThisWindow(window->winId(), TRUE);
		}
		LockSetForegroundWindow(LSFW_LOCK);
	}

	return ret;
}

/*
 * Registry root key
 */
static HKEY mixp_reg_root(int rootKey)
{
	switch(rootKey)
	{
		case mixp_root_classes: return HKEY_CLASSES_ROOT;  break;
		case mixp_root_user:    return HKEY_CURRENT_USER;  break;
		case mixp_root_machine: return HKEY_LOCAL_MACHINE; break;
		default: throw "Unknown root reg value was specified!";
	}
}

/*
 * Write registry value
 */
bool mixp_reg_value_write(int rootKey, const QString &keyName, const QString &valueName, const quint32 value)
{
	bool success = false; HKEY hKey = NULL;
	if(RegCreateKeyEx(mixp_reg_root(rootKey), QWCHAR(keyName), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey, valueName.isEmpty() ? NULL : QWCHAR(valueName), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(quint32)) == ERROR_SUCCESS)
		{
			success = true;
		}
		CloseHandle(hKey);
	}
	return success;
}

/*
 * Write registry value
 */
bool mixp_reg_value_write(int rootKey, const QString &keyName, const QString &valueName, const QString &value)
{
	bool success = false; HKEY hKey = NULL;
	if(RegCreateKeyEx(mixp_reg_root(rootKey), QWCHAR(keyName), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey, valueName.isEmpty() ? NULL : QWCHAR(valueName), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.utf16()), (value.length() + 1) * sizeof(wchar_t)) == ERROR_SUCCESS)
		{
			success = true;
		}
		CloseHandle(hKey);
	}
	return success;
}

/*
 * Read registry value
 */
bool mixp_reg_value_read(int rootKey, const QString &keyName, const QString &valueName, quint32 &value)
{
	bool success = false; HKEY hKey = NULL;
	if(RegOpenKeyEx(mixp_reg_root(rootKey), QWCHAR(keyName), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD size = sizeof(quint32), type = -1;
		if(RegQueryValueEx(hKey, valueName.isEmpty() ? NULL : QWCHAR(valueName), 0, &type, reinterpret_cast<BYTE*>(&value), &size) == ERROR_SUCCESS)
		{
			success = (type == REG_DWORD);
		}
		CloseHandle(hKey);
	}
	return success;
}

/*
 * Delete registry key
 */
bool mixp_reg_key_delete(int rootKey, const QString &keyName)
{
	return (SHDeleteKey( mixp_reg_root(rootKey), QWCHAR(keyName)) == ERROR_SUCCESS);
}

/*
 * Shell notification
 */
void mixp_shell_change_notification(void)
{
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

/*
 * Global init
 */
void _mixp_global_init(void)
{
	g_mixp_known_folder.initialized = false;
}
