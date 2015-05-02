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

//MUTils
#include <MUtils/Startup.h>
#include <MUtils/Version.h>

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

static int mixp_main(int &argc, char **argv)
{
	//Print the logo
	qDebug("MediaInfoXP v%u.%02u, built on %s at %s.", mixp_versionMajor, mixp_versionMinor, mixp_buildDate, mixp_buildTime);
	qDebug("Copyright (c) 2004-%s LoRd_MuldeR <mulder2@gmx.de>. Some rights reserved.", &mixp_buildDate[7]);
	qDebug("Built with Qt v%s, running with Qt v%s.\n", QT_VERSION_STR, qVersion());

	//Print library version
	qDebug("This application is powerd by MUtils library v%u.%02u (%s, %s).\n", MUtils::Version::lib_version_major(), MUtils::Version::lib_version_minor(), MUTILS_UTF8(MUtils::Version::lib_build_date().toString(Qt::ISODate)), MUTILS_UTF8(MUtils::Version::lib_build_time().toString(Qt::ISODate)));

	//Create application
	QScopedPointer<QApplication> application(new QApplication(argc, argv));

	//Create IPC
	QScopedPointer<IPC> ipc(new IPC());
	
	//Is this the *first* instance?
	if(ipc->initialize() == 0)
	{
		//We are *not* the first instance -> pass all file names to the running instance
		const QStringList arguments = qApp->arguments();
		bool bHaveFile = false;
		for(QStringList::ConstIterator iter = arguments.constBegin(); iter != arguments.constEnd(); iter++)
		{
			if(QString::compare(*iter, "--open", Qt::CaseInsensitive) == 0)
			{
				if(++iter != arguments.constEnd())
				{
					if(ipc->sendAsync(*iter))
					{
						bHaveFile = true;
						continue;
					}
				}
				break;
			}
		}
		//If no file was sent, we will at least try to bring the other instance to front
		if(!bHaveFile)
		{
			ipc->sendAsync("?");
		}

		return 42;
	}

	//Get temp folder
	const QString tempFolder =  MUtils::temp_folder();
	qDebug("TEMP folder is:\n%s\n", QDir::toNativeSeparators(tempFolder).toUtf8().constData());

	//Create main window
	QScopedPointer<CMainWindow> mainWindow(new CMainWindow(tempFolder, ipc.data()));
	mainWindow->show();

	//Run application
	const int exit_code = application->exec();
	qDebug("\nTime to say goodbye... (%d)\n", exit_code);
	
	//Stop IPC
	ipc->stopListening();

	return exit_code;
}

///////////////////////////////////////////////////////////////////////////////
// Applicaton entry point
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	MUtils::Startup::startup(argc, argv, mixp_main, "MediaInfoXP", false);
}
