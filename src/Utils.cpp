///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2013 LoRd_MuldeR <MuldeR2@GMX.de>
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

//Win32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <Objbase.h>
#include <Psapi.h>

/*
 * Try to lock folder
 */
QString mixp_tryLockFolder(const QString &folderPath, QFile **lockfile)
{
	const QString SUB_FOLDER = QUuid::createUuid().toString();
	const QByteArray WRITE_TEST_DATA = QByteArray("Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.");

	QDir folder(folderPath);
	if(!folder.exists())
	{
		folder.mkdir(".");
	}

	if(folder.exists())
	{
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

	return QString();
}

/*
 * Get AppData folder
 */
QString mixp_getAppDataFolder(void)
{
	typedef HRESULT (WINAPI *SHGetKnownFolderPathFun)(__in const GUID &rfid, __in DWORD dwFlags, __in HANDLE hToken, __out PWSTR *ppszPath);
	typedef HRESULT (WINAPI *SHGetFolderPathFun)(__in HWND hwndOwner, __in int nFolder, __in HANDLE hToken, __in DWORD dwFlags, __out LPWSTR pszPath);

	static const int CSIDL_LOCAL_APPDATA = 0x001c;
	static const GUID GUID_LOCAL_APPDATA = {0xF1B32785,0x6FBA,0x4FCF,{0x9D,0x55,0x7B,0x8E,0x7F,0x15,0x70,0x91}};

	SHGetKnownFolderPathFun SHGetKnownFolderPathPtr = NULL;
	SHGetFolderPathFun SHGetFolderPathPtr = NULL;

	QLibrary kernel32Lib("shell32.dll");
	if(kernel32Lib.load())
	{
		SHGetKnownFolderPathPtr = (SHGetKnownFolderPathFun) kernel32Lib.resolve("SHGetKnownFolderPath");
		SHGetFolderPathPtr = (SHGetFolderPathFun) kernel32Lib.resolve("SHGetFolderPathW");
	}

	QString folder;

	if(SHGetKnownFolderPathPtr)
	{
		qDebug("SHGetKnownFolderPathPtr()\n");
		WCHAR *path = NULL;
		if(SHGetKnownFolderPathPtr(GUID_LOCAL_APPDATA, 0x00008000, NULL, &path) == S_OK)
		{
			QDir folderTemp = QDir(QDir::fromNativeSeparators(QString::fromUtf16(reinterpret_cast<const unsigned short*>(path))));
			if(!folderTemp.exists())
			{
				folderTemp.mkpath(".");
			}
			if(folderTemp.exists())
			{
				folder = folderTemp.canonicalPath();
			}
			CoTaskMemFree(path);
		}
	}
	else if(SHGetFolderPathPtr)
	{
		qDebug("SHGetFolderPathPtr()\n");
		WCHAR *path = new WCHAR[4096];
		if(SHGetFolderPathPtr(NULL, CSIDL_LOCAL_APPDATA, NULL, NULL, path) == S_OK)
		{
			QDir folderTemp = QDir(QDir::fromNativeSeparators(QString::fromUtf16(reinterpret_cast<const unsigned short*>(path))));
			if(!folderTemp.exists())
			{
				folderTemp.mkpath(".");
			}
			if(folderTemp.exists())
			{
				folder = folderTemp.canonicalPath();
			}
		}
		delete [] path;
	}

	return folder;
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

	qWarning("Failed to init %%TEMP%%, falling back to %%LOCALAPPDATA%%\n");

	//Create TEMP folder in %LOCALAPPDATA%
	QString localAppDataPath = mixp_getAppDataFolder();
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

	return QString();
}

/*
 * Clean folder
 */
void mixp_clean_folder(const QString &folderPath)
{
	QDir tempFolder(folderPath);
	QFileInfoList entryList = tempFolder.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

	for(int i = 0; i < entryList.count(); i++)
	{
		if(entryList.at(i).isDir())
		{
			mixp_clean_folder(entryList.at(i).canonicalFilePath());
		}
		else
		{
			for(int j = 0; j < 5; j++)
			{
				if(QFile::remove(entryList.at(i).canonicalFilePath()))
				{
					break;
				}
			}
		}
	}
	
	tempFolder.rmdir(".");
}

/*
 * Clean folder
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
 * Clean folder
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
