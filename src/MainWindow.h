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

#pragma once

#include <QMainWindow>
#include <QDate>

//UIC forward declartion
namespace Ui {
	class MainWindow;
}
class QProcess;
class QLabel;
typedef void *HANDLE;

//MainWindow class
class CMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	CMainWindow(const QString &tempFolder, QWidget *parent = 0);
	~CMainWindow(void);

private slots:
	void analyzeButtonClicked(void);
	void saveButtonClicked(void);
	void copyToClipboardButtonClicked(void);
	void clearButtonClicked(void);
	void outputAvailable(void);
	void processFinished(void);
	void linkTriggered(void);
	void showAboutScreen(void);
	void handleDroppedFile(void);

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void closeEvent(QCloseEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::MainWindow *ui; //for Qt UIC
	
	const QString &m_tempFolder;
	bool m_firstShow;
	QString m_mediaInfoPath;
	HANDLE m_mediaInfoHandle;
	QProcess *m_process;
	QLabel *m_floatingLabel;
	QString m_droppedFile;
	QStringList m_outputLines;

	QString getMediaInfoPath(void);
	bool analyzeFile(const QString &filePath);
};
