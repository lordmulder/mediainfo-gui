///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2014 LoRd_MuldeR <MuldeR2@GMX.de>
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
#include <signal.h>

#pragma intrinsic(_InterlockedExchange)

//Qt
#include <QApplication>
#include <QDir>

//Win32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#ifdef QT_NODLL
	#include <QtPlugin>
	Q_IMPORT_PLUGIN(qico)
	Q_IMPORT_PLUGIN(qsvg)
	Q_IMPORT_PLUGIN(qtga)
#endif

#include "Config.h"
#include "MainWindow.h"
#include "IPC.h"
#include "Utils.h"

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

	qDebug("MediaInfoXP v%u.%02u, built on %s at %s.", mixp_versionMajor, mixp_versionMinor, mixp_buildDate, mixp_buildTime);
	qDebug("Copyright (c) 2004-%s LoRd_MuldeR <mulder2@gmx.de>. Some rights reserved.", &mixp_buildDate[7]);
	qDebug("Built with Qt v%s, running with Qt v%s.\n", QT_VERSION_STR, qVersion());

	//Initialize IPC
	IPC *ipc = new IPC();
	if(ipc->init() == 0)
	{
		ipc->sendAsync("Test Hello World 123!");
		return 0;
	}

	QString test;
	qDebug("Awaiting data from other instance...");
	if(ipc->popStr(test))
	{
		qDebug("Got the data: %s\n", test.toUtf8().constData());
	}

	QFile *lockFile = NULL;

	//Get temp folder
	const QString tempFolder = mixp_getTempFolder(&lockFile);
	if(tempFolder.isEmpty())
	{
		qFatal("Failed to determine TEMP folder!");
		MIXP_DELETE_OBJ(ipc);
		return 1;
	}
	
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
	MIXP_DELETE_OBJ(ipc);

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

static void mixp_signal_handler(int signal_num)
{
	signal(signal_num, mixp_signal_handler);
	mixp_fatal_exit(L"Signal handler invoked, application will exit!");
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
		_mixp_global_init();
		return _main(argc, argv);
	}
	else
	{
		__try
		{
			SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
			SetUnhandledExceptionFilter(mixp_exception_handler);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
			_set_invalid_parameter_handler(mixp_invalid_param_handler);
	
			static const int signal_num[6] = { SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM };

			for(size_t i = 0; i < 6; i++)
			{
				signal(signal_num[i], mixp_signal_handler);
			}

			_mixp_global_init();
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
