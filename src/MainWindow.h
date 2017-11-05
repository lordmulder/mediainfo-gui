///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2017 LoRd_MuldeR <MuldeR2@GMX.de>
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

#include <QMainWindow>
#include <QDate>

//UIC forward declartion
namespace Ui
{
	class MainWindow;
}
namespace MUtils
{
	class IPCChannel;
}
class QProcess;
class QLabel;
class QFile;
class IPCReceiveThread;

//MainWindow class
class CMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	CMainWindow(const QString &tempFolder, MUtils::IPCChannel *const ipc, QWidget *parent = 0);
	~CMainWindow(void);

private slots:
	void analyzeFiles(void);
	void analyzeNextFile(void);
	void analyzeButtonClicked(void);
	void saveButtonClicked(void);
	void copyToClipboardButtonClicked(void);
	void clearButtonClicked(void);
	void outputAvailable(void);
	void processFinished(void);
	void linkTriggered(void);
	void showAboutScreen(void);
	void updateSize(void);
	void initShellExtension(void);
	void updateShellExtension(bool checked);
	void updateLineWrapping(bool checked);
	void toggleOutputOption(bool checked);
	void received(const quint32 &command, const QString &message);

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void closeEvent(QCloseEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual bool eventFilter(QObject *o, QEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

private:
	enum
	{
		APP_STATUS_STARTING = 0,
		APP_STATUS_IDLE = 1,
		APP_STATUS_AWAITING = 2,
		APP_STATUS_WORKING = 3,
		APP_STATUS_BLOCKED = 4
	}
	status_t;
	
	Ui::MainWindow *ui; //for Qt UIC
	
	int m_status;
	const QString &m_tempFolder;

	QScopedPointer<IPCReceiveThread> m_ipcThread;
	QScopedPointer<QFile> m_mediaInfoHandle;
	QScopedPointer<QProcess> m_process;
	QScopedPointer<QLabel> m_floatingLabel;
	QStringList m_pendingFiles;
	QStringList m_outputLines;

	QPair<QString, const char*> getMediaInfoArch(void);
	void selfTest(void);
	QString getMediaInfoPath(void);

	static QStringList escapeHtmlLines(const QStringList &strings);
	static QString reformatXml(const QString &input);
};
