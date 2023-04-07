///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2023 LoRd_MuldeR <MuldeR2@GMX.de>
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

#define MIXP_CREATE_CONFIG 1

//MUTils
#include <MUtils/Startup.h>
#include <MUtils/IPCChannel.h>
#include <MUtils/Version.h>
#include <MUtils/OSSupport.h>

//Qt
#include <QApplication>
#include <QDir>
#include <QHash>

#ifdef QT_NODLL
	#include <QtPlugin>
	Q_IMPORT_PLUGIN(qico)
	Q_IMPORT_PLUGIN(qsvg)
	Q_IMPORT_PLUGIN(qtga)
#endif

//Internal
#include "Config.h"
#include "MainWindow.h"
#include "IPC.h"

///////////////////////////////////////////////////////////////////////////////
// Multi-instance handling
///////////////////////////////////////////////////////////////////////////////

static void mixp_handle_multi_instance(MUtils::IPCChannel *const ipcChannel)
{
	bool bHaveFile = false;

	//We are *not* the first instance -> pass all file names to the running instance
	const MUtils::OS::ArgumentMap arguments = MUtils::OS::arguments();
	const QStringList files = arguments.values("open");
	for(QStringList::ConstIterator iter = files.constBegin(); iter != files.constEnd(); iter++)
	{
		if(IPC::sendAsync(ipcChannel, IPC::COMMAND_OPEN, *iter))
		{
			bHaveFile = true;
		}
	}

	//If no file was sent, we will at least try to bring the other instance to front
	if(!bHaveFile)
	{
		IPC::sendAsync(ipcChannel, IPC::COMMAND_PING, "?");
	}
}

///////////////////////////////////////////////////////////////////////////////
// MAIN function
///////////////////////////////////////////////////////////////////////////////

static int mixp_main(int &argc, char **argv)
{
	//Print the logo
	qDebug("MediaInfoXP v%u.%02u, built on %s at %s.", g_mixp_versionMajor, g_mixp_versionMinor, g_mixp_buildDate, g_mixp_buildTime);
	qDebug("Copyright (c) 2004-%s LoRd_MuldeR <mulder2@gmx.de>. Some rights reserved.", &g_mixp_buildDate[7]);
	qDebug("Built with Qt v%s, running with Qt v%s.\n", QT_VERSION_STR, qVersion());

	//Print library version
	qDebug("This application is powerd by MUtils library v%u.%02u (%s, %s).\n", MUtils::Version::lib_version_major(), MUtils::Version::lib_version_minor(), MUTILS_UTF8(MUtils::Version::lib_build_date().toString(Qt::ISODate)), MUTILS_UTF8(MUtils::Version::lib_build_time().toString(Qt::ISODate)));

	//Create application
	QScopedPointer<QApplication> application(new QApplication(argc, argv));

	//Create IPC
	QScopedPointer<MUtils::IPCChannel> ipcChannel(new MUtils::IPCChannel("mediainfo-xp", qHash(QString("%0@%1").arg(QString::fromLatin1(g_mixp_buildDate), QString::fromLatin1(g_mixp_buildTime))), "instance"));
	const int ipcMode = ipcChannel->initialize();
	if((ipcMode != MUtils::IPCChannel::RET_SUCCESS_MASTER) && (ipcMode != MUtils::IPCChannel::RET_SUCCESS_SLAVE))
	{
		qFatal("The IPC initialization has failed!");
		return EXIT_FAILURE;
	}

	//Handle multiple instances
	if(ipcMode == MUtils::IPCChannel::RET_SUCCESS_SLAVE)
	{
		mixp_handle_multi_instance(ipcChannel.data());
		return EXIT_SUCCESS;
	}

	const QString baseFolder = QDir(QCoreApplication::applicationDirPath()).canonicalPath();
	qDebug("Base directory is:\n%s\n", MUTILS_UTF8(QDir::toNativeSeparators(baseFolder)));

	//Get temp folder
	const QString tempFolder =  MUtils::temp_folder();
	qDebug("Temp directory is:\n%s\n", MUTILS_UTF8(QDir::toNativeSeparators(tempFolder)));

	//Create main window
	QScopedPointer<CMainWindow> mainWindow(new CMainWindow(baseFolder, tempFolder, ipcChannel.data()));
	mainWindow->show();

	//Run application
	const int exit_code = application->exec();
	qDebug("\nTime to say goodbye... (%d)\n", exit_code);

	return exit_code;
}

///////////////////////////////////////////////////////////////////////////////
// Applicaton entry point
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	MUtils::Startup::startup(argc, argv, mixp_main, "MediaInfoXP", MIXP_DEBUG);
}

extern "C"
{
	int mainCRTStartup(void);

	int mxp_entry_point(void)
	{
		return mainCRTStartup();
	}
}
