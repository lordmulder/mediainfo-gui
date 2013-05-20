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

//StdLib
#include <cstdio>
#include <iostream>
#include <fstream>
#include <ctime>

#pragma intrinsic(_InterlockedExchange)

//Qt
#include <QApplication>
#include <QMutex>
#include <QUuid>
#include <QDir>
#include <QLibrary>

//Win32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <Objbase.h>

#ifdef QT_NODLL
	#include <QtPlugin>
	Q_IMPORT_PLUGIN(qico)
	Q_IMPORT_PLUGIN(qsvg)
	Q_IMPORT_PLUGIN(qtga)
#endif

#include "Config.h"
#include "MainWindow.h"

///////////////////////////////////////////////////////////////////////////////
// Debug Console
///////////////////////////////////////////////////////////////////////////////

bool g_bHaveConsole = false;

static void init_console(void)
{
	if(AllocConsole())
	{
		int hCrtStdOut = _open_osfhandle((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE), _O_WRONLY);
		int hCrtStdErr = _open_osfhandle((intptr_t) GetStdHandle(STD_ERROR_HANDLE),  _O_WRONLY);
		FILE *hfStdOut = (hCrtStdOut >= 0) ? _fdopen(hCrtStdOut, "wb") : NULL;
		FILE *hfStdErr = (hCrtStdErr >= 0) ? _fdopen(hCrtStdErr, "wb") : NULL;
		if(hfStdOut) { *stdout = *hfStdOut; std::cout.rdbuf(new std::filebuf(hfStdOut)); }
		if(hfStdErr) { *stderr = *hfStdErr; std::cerr.rdbuf(new std::filebuf(hfStdErr)); }

		HWND hwndConsole = GetConsoleWindow();
		if((hwndConsole != NULL) && (hwndConsole != INVALID_HANDLE_VALUE))
		{
			HMENU hMenu = GetSystemMenu(hwndConsole, 0);
			EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
			RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			SetWindowPos(hwndConsole, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
			SetWindowLong(hwndConsole, GWL_STYLE, GetWindowLong(hwndConsole, GWL_STYLE) & (~WS_MAXIMIZEBOX) & (~WS_MINIMIZEBOX));
			SetWindowPos(hwndConsole, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
		}

		g_bHaveConsole = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Detect TEMP folder
///////////////////////////////////////////////////////////////////////////////

QString mixp_getAppDataFolder(void)
{
	typedef HRESULT (WINAPI *SHGetKnownFolderPathFun)(__in const GUID &rfid, __in DWORD dwFlags, __in HANDLE hToken, __out PWSTR *ppszPath);
	typedef HRESULT (WINAPI *SHGetFolderPathFun)(__in HWND hwndOwner, __in int nFolder, __in HANDLE hToken, __in DWORD dwFlags, __out LPWSTR pszPath);

	static const int CSIDL_LOCAL_APPDATA = 0x001c;
	static const GUID GUID_LOCAL_APPDATA = {0xF1B32785,0x6FBA,0x4FCF,{0x9D,0x55,0x7B,0x8E,0x7F,0x15,0x70,0x91}};

	static SHGetKnownFolderPathFun SHGetKnownFolderPathPtr = NULL;
	static SHGetFolderPathFun SHGetFolderPathPtr = NULL;

	if((!SHGetKnownFolderPathPtr) && (!SHGetFolderPathPtr))
	{
		QLibrary kernel32Lib("shell32.dll");
		if(kernel32Lib.load())
		{
			SHGetKnownFolderPathPtr = (SHGetKnownFolderPathFun) kernel32Lib.resolve("SHGetKnownFolderPath");
			SHGetFolderPathPtr = (SHGetFolderPathFun) kernel32Lib.resolve("SHGetFolderPathW");
		}
	}

	QString folder;

	if(SHGetKnownFolderPathPtr)
	{
		WCHAR *path = NULL;
		if(SHGetKnownFolderPathPtr(GUID_LOCAL_APPDATA, 0x00008000, NULL, &path) == S_OK)
		{
			//MessageBoxW(0, path, L"SHGetKnownFolderPath", MB_TOPMOST);
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
		WCHAR *path = new WCHAR[4096];
		if(SHGetFolderPathPtr(NULL, CSIDL_LOCAL_APPDATA, NULL, NULL, path) == S_OK)
		{
			//MessageBoxW(0, path, L"SHGetFolderPathW", MB_TOPMOST);
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

QString mixp_getTempFolder(QFile **lockfile)
{
	*lockfile = NULL;
	QString tempFolder;

	static const char *TEMP_STR = "Temp";
	const QByteArray WRITE_TEST_DATA = QByteArray("Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.");
	const QString SUB_FOLDER = QUuid::createUuid().toString();

	//Try the %TMP% or %TEMP% directory first
	QDir temp = QDir::temp();
	if(temp.exists())
	{
		temp.mkdir(SUB_FOLDER);
		if(temp.cd(SUB_FOLDER) && temp.exists())
		{
			QFile *testFile = new QFile(QString("%1/~lock.tmp").arg(temp.canonicalPath()));
			if(testFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
			{
				if(testFile->write(WRITE_TEST_DATA) >= WRITE_TEST_DATA.size())
				{
					*lockfile = testFile; testFile = NULL;
					tempFolder = temp.canonicalPath();
				}
				if(testFile) testFile->remove();
				MIXP_DELETE_OBJ(testFile);
			}
		}
		if(!tempFolder.isEmpty())
		{
			return tempFolder;
		}
	}

	//Create TEMP folder in %LOCALAPPDATA%
	QDir localAppData = QDir(mixp_getAppDataFolder());
	if(!localAppData.path().isEmpty())
	{
		if(!localAppData.exists())
		{
			localAppData.mkpath(".");
		}
		if(localAppData.exists())
		{
			if(!localAppData.entryList(QDir::AllDirs).contains(TEMP_STR, Qt::CaseInsensitive))
			{
				localAppData.mkdir(TEMP_STR);
			}
			if(localAppData.cd(TEMP_STR) && localAppData.exists())
			{
				localAppData.mkdir(SUB_FOLDER);
				if(localAppData.cd(SUB_FOLDER) && localAppData.exists())
				{
					QFile *testFile = new QFile(QString("%1/~lock.tmp").arg(localAppData.canonicalPath()));
					if(testFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
					{
						if(testFile->write(WRITE_TEST_DATA) >= WRITE_TEST_DATA.size())
						{
							*lockfile = testFile; testFile = NULL;
							tempFolder = localAppData.canonicalPath();
						}
						if(testFile) testFile->remove();
						MIXP_DELETE_OBJ(testFile);
					}
				}
			}
		}
		if(!tempFolder.isEmpty())
		{
			return tempFolder;
		}
	}

	qFatal("Failed to determine TEMP folder!");
	return QString();
}

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

///////////////////////////////////////////////////////////////////////////////
// MAIN function
///////////////////////////////////////////////////////////////////////////////

int mixp_main(int argc, char* argv[])
{
	//Show console?
	bool bConsole = MIXP_CONSOLE;
	for(int i = 1; i < argc; i++)
	{
		if(_stricmp(argv[i], "--console") == 0) bConsole = true;
		if(_stricmp(argv[i], "--no-console") == 0) bConsole = false;
	}

	if(bConsole) init_console();

	qDebug("MediaInfoXP [%s]", mixp_buildDate);
	qDebug("Copyright (c) 2004-%s LoRd_MuldeR <mulder2@gmx.de>. Some rights reserved.", &mixp_buildDate[7]);
	qDebug("Built with Qt v%s, running with Qt v%s.\n", QT_VERSION_STR, qVersion());

	QFile *lockFile = NULL;

	//Get temp folder
	const QString tempFolder = mixp_getTempFolder(&lockFile);
	qDebug("TEMP folder is:\n%s\n", QDir::toNativeSeparators(tempFolder).toUtf8().constData());

	//Create application
	QApplication *application = new QApplication(argc, argv);
	application->setWindowIcon(QIcon(":/QtTestApp.ico"));

	//Create main window
	CMainWindow *mainWindow = new CMainWindow(tempFolder);
	mainWindow->show();

	//Run application
	const int exit_code = application->exec();
	qDebug("\nTime to say goodbye... (%d)\n", exit_code);
	
	//Clean up
	MIXP_DELETE_OBJ(mainWindow);
	MIXP_DELETE_OBJ(application);
	if(lockFile) lockFile->remove();
	MIXP_DELETE_OBJ(lockFile);
	mixp_clean_folder(tempFolder);

	return exit_code;
}

///////////////////////////////////////////////////////////////////////////////
// Error handlers
///////////////////////////////////////////////////////////////////////////////

void mixp_fatal_exit(const wchar_t *msg)
{
	static volatile long s_lock = 0L;
	if(_InterlockedExchange(&s_lock, 1L) == 0)
	{
		FatalAppExitW(0, msg);
		TerminateProcess(GetCurrentProcess(), -1);
	}
}

LONG WINAPI mixp_exception_handler(__in struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	mixp_fatal_exit(L"Unhandeled exception handler invoked, application will exit!");
	return LONG_MAX;
}

void mixp_invalid_param_handler(const wchar_t* exp, const wchar_t* fun, const wchar_t* fil, unsigned int, uintptr_t)
{
	mixp_fatal_exit(L"Invalid parameter handler invoked, application will exit!");
}

///////////////////////////////////////////////////////////////////////////////
// Message Handler
///////////////////////////////////////////////////////////////////////////////

static void mixp_console_color(FILE* file, WORD attributes)
{
	const HANDLE hConsole = (HANDLE)(_get_osfhandle(_fileno(file)));
	if((hConsole != NULL) && (hConsole != INVALID_HANDLE_VALUE))
	{
		SetConsoleTextAttribute(hConsole, attributes);
	}
}

void mixp_message_handler(QtMsgType type, const char *msg)
{
	if(g_bHaveConsole)
	{
		static const char *GURU_MEDITATION = "\n\nGURU MEDITATION !!!\n\n";
	
		static volatile long s_lock = 0L;
		while(_InterlockedExchange(&s_lock, 1L) != 0) __noop;

		UINT oldOutputCP = GetConsoleOutputCP();
		if(oldOutputCP != CP_UTF8) SetConsoleOutputCP(CP_UTF8);

		switch(type)
		{
		case QtCriticalMsg:
		case QtFatalMsg:
			fflush(stdout);
			fflush(stderr);
			mixp_console_color(stderr, FOREGROUND_RED | FOREGROUND_INTENSITY);
			fprintf(stderr, GURU_MEDITATION);
			fprintf(stderr, "%s\n", msg);
			fflush(stderr);
			break;
		case QtWarningMsg:
			mixp_console_color(stderr, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			fprintf(stderr, "%s\n", msg);
			fflush(stderr);
			break;
		default:
			mixp_console_color(stderr, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			fprintf(stderr, "%s\n", msg);
			fflush(stderr);
			break;
		}
	
		mixp_console_color(stderr, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		if(oldOutputCP != CP_UTF8) SetConsoleOutputCP(oldOutputCP);

		_InterlockedExchange(&s_lock, 0L);
	}

	if(type == QtCriticalMsg || type == QtFatalMsg)
	{
		mixp_fatal_exit(L"The application has encountered a critical error and will exit now!");
	}
}

///////////////////////////////////////////////////////////////////////////////
// Applicaton entry point
///////////////////////////////////////////////////////////////////////////////

static int _main(int argc, char* argv[])
{
	if(MIXP_DEBUG)
	{
		qInstallMsgHandler(mixp_message_handler);
		return mixp_main(argc, argv);
	}
	else
	{
		int iResult = -1;
		try
		{
			qInstallMsgHandler(mixp_message_handler);
			iResult = mixp_main(argc, argv);
		}
		catch(char *error)
		{
			fflush(stdout);
			fflush(stderr);
			fprintf(stderr, "\nGURU MEDITATION !!!\n\nException error message: %s\n", error);
			mixp_fatal_exit(L"Unhandeled C++ exception error, application will exit!");
		}
		catch(int error)
		{
			fflush(stdout);
			fflush(stderr);
			fprintf(stderr, "\nGURU MEDITATION !!!\n\nException error code: 0x%X\n", error);
			mixp_fatal_exit(L"Unhandeled C++ exception error, application will exit!");
		}
		catch(...)
		{
			fflush(stdout);
			fflush(stderr);
			fprintf(stderr, "\nGURU MEDITATION !!!\n");
			mixp_fatal_exit(L"Unhandeled C++ exception error, application will exit!");
		}
		return iResult;
	}
}

int main(int argc, char* argv[])
{
	if(MIXP_DEBUG)
	{
		return _main(argc, argv);
	}
	else
	{
		__try
		{
			SetUnhandledExceptionFilter(mixp_exception_handler);
			_set_invalid_parameter_handler(mixp_invalid_param_handler);
			return _main(argc, argv);
		}
		__except(1)
		{
			fflush(stdout);
			fflush(stderr);
			fprintf(stderr, "\nGURU MEDITATION !!!\n\nUnhandeled structured exception error! [code: 0x%X]\n", GetExceptionCode());
			mixp_fatal_exit(L"Unhandeled structured exception error, application will exit!");
		}
	}
}
