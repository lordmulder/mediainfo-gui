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

#pragma once

#include <QMainWindow>
#include <QDate>

//UIC forward declartion
namespace Ui {
	class MainWindow;
}
class QProcess;
class QLabel;
class QFile;
class mixp_icon_t;

//MainWindow class
class CMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	CMainWindow(const QString &tempFolder, QWidget *parent = 0);
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

protected:
	virtual void showEvent(QShowEvent *event);
	virtual void closeEvent(QCloseEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual bool eventFilter(QObject *o, QEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

private:
	Ui::MainWindow *ui; //for Qt UIC
	
	const QString &m_tempFolder;
	bool m_firstShow;
	QFile *m_mediaInfoHandle;
	QProcess *m_process;
	QLabel *m_floatingLabel;
	QStringList m_pendingFiles;
	QStringList m_outputLines;
	mixp_icon_t *m_icon;
	
	const QList<QPair<const QString, const QString>> m_htmlEscape;

	QString getMediaInfoPath(void);
	void escapeHtmlChars(QStringList &strings);
};
