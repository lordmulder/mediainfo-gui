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

#include "MainWindow.h"

//UIC includes
#include "../tmp/Common/uic/Dialog.h"

//Qt includes
#include <QMessageBox>
#include <QFileDialog>
#include <QStyleFactory>
#include <QTimer>
#include <QResource>
#include <QProcess>
#include <QCloseEvent>
#include <QScrollBar>
#include <QDesktopServices>

//Win32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//Internal
#include "Config.h"

//Macros
#define SET_FONT_BOLD(WIDGET,BOLD) { QFont _font = WIDGET->font(); _font.setBold(BOLD); WIDGET->setFont(_font); }
#define SET_TEXT_COLOR(WIDGET,COLOR) { QPalette _palette = WIDGET->palette(); _palette.setColor(QPalette::WindowText, (COLOR)); _palette.setColor(QPalette::Text, (COLOR)); WIDGET->setPalette(_palette); }

////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////

const char *LINK_MULDER = "http://muldersoft.com/";
const char *LINK_MEDIAINFO = "http://mediainfo.sourceforge.net/en";
const char *LINK_DISCUSS = "http://forum.doom9.org/showthread.php?t=96516";

CMainWindow::CMainWindow(const QString &tempFolder, QWidget *parent)
:
	QMainWindow(parent),
	m_tempFolder(tempFolder),
	m_firstShow(true),
	ui(new Ui::MainWindow)
{
	//Init UI
	qApp->setStyle(QStyleFactory::create("plastique"));
	ui->setupUi(this);
	setMinimumSize(this->size());

	//Setup links
	ui->actionLink_MuldeR->setData(QVariant(QString::fromLatin1(LINK_MULDER)));
	ui->actionLink_MediaInfo->setData(QVariant(QString::fromLatin1(LINK_MEDIAINFO)));
	ui->actionLink_Discuss->setData(QVariant(QString::fromLatin1(LINK_DISCUSS)));

	//Setup connections
	connect(ui->analyzeButton, SIGNAL(clicked()), this, SLOT(analyzeButtonClicked()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(analyzeButtonClicked()));
	connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clearButtonClicked()));
	connect(ui->actionLink_MuldeR, SIGNAL(triggered()), this, SLOT(linkTriggered()));
	connect(ui->actionLink_MediaInfo, SIGNAL(triggered()), this, SLOT(linkTriggered()));
	connect(ui->actionLink_Discuss, SIGNAL(triggered()), this, SLOT(linkTriggered()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutScreen()));
	ui->versionLabel->installEventFilter(this);

	//Create label
	m_floatingLabel = new QLabel(ui->textBrowser);
	m_floatingLabel->setText(">> You can drop any type of media files here <<");
	m_floatingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_floatingLabel->show();
	SET_TEXT_COLOR(m_floatingLabel, Qt::darkGray);
	SET_FONT_BOLD(m_floatingLabel, true);

	//Clear
	m_mediaInfoPath.clear();
	m_mediaInfoHandle = INVALID_HANDLE_VALUE;
	m_process = NULL;
}

////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////

CMainWindow::~CMainWindow(void)
{
	if(m_mediaInfoHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_mediaInfoHandle);
		m_mediaInfoHandle = INVALID_HANDLE_VALUE;
	}
	MIXP_DELETE_OBJ(m_process);
	MIXP_DELETE_OBJ(m_floatingLabel);
}

////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////

void CMainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
	resize(this->minimumSize());
	ui->versionLabel->setText(QString("v%1 (%2)").arg(QString().sprintf("%u.%02u", mixp_versionMajor, mixp_versionMinor), getBuildDate().toString(Qt::ISODate)));
	resizeEvent(NULL);
	setAcceptDrops(true);

	if(m_firstShow)
	{
		const QStringList arguments = qApp->arguments();
		for(QStringList::ConstIterator iter = arguments.constBegin(); iter != arguments.constEnd(); iter++)
		{
			if(QString::compare(*iter, "--open", Qt::CaseInsensitive) == 0)
			{
				if(++iter != arguments.constEnd())
				{
					QFileInfo currentFile = QFileInfo(*iter);
					if(currentFile.exists() && currentFile.isFile())
					{
						m_droppedFile = currentFile.canonicalFilePath();
						QTimer::singleShot(0, this, SLOT(handleDroppedFile()));
						break;
					}
					continue;
				}
				break;
			}
		}
		m_firstShow = false;
	}
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
	if(m_process)
	{
		if(m_process->state() != QProcess::NotRunning)
		{
			event->ignore();
		}
	}
}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
	if(event)
	{
		QMainWindow::resizeEvent(event);
	}
	if(const QWidget *const viewPort = ui->textBrowser->viewport())
	{
		m_floatingLabel->setGeometry(0, 0, viewPort->width(), viewPort->height());
	}
}

void CMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	bool accept[2] = {false, false};

	foreach(const QString &fmt, event->mimeData()->formats())
	{
		accept[0] = accept[0] || fmt.contains("text/uri-list", Qt::CaseInsensitive);
		accept[1] = accept[1] || fmt.contains("FileNameW", Qt::CaseInsensitive);
	}

	if(accept[0] && accept[1])
	{
		event->acceptProposedAction();
	}
}

void CMainWindow::dropEvent(QDropEvent *event)
{
	QStringList droppedFiles;
	QList<QUrl> urls = event->mimeData()->urls();

	while(!urls.isEmpty())
	{
		QUrl currentUrl = urls.takeFirst();
		QFileInfo file(currentUrl.toLocalFile());
		if(file.exists() && file.isFile())
		{
			m_droppedFile = file.canonicalFilePath();
			QTimer::singleShot(0, this, SLOT(handleDroppedFile()));
			break;
		}
	}
}

bool CMainWindow::eventFilter(QObject *o, QEvent *e)
{
	if((o == ui->versionLabel) && (e->type() == QEvent::MouseButtonPress))
	{
		QTimer::singleShot(0, this, SLOT(showAboutScreen()));
		return true;
	}
	return QMainWindow::eventFilter(o, e);
}

////////////////////////////////////////////////////////////
// SLOTS
////////////////////////////////////////////////////////////

void CMainWindow::analyzeButtonClicked(void)
{
	const QString selectedFile = QFileDialog::getOpenFileName(this, tr("Select file to analyze..."), QString(), tr("All supported media files (*.*)"));
	if(!selectedFile.isEmpty())
	{
		analyzeFile(selectedFile);
	}
}

void CMainWindow::clearButtonClicked(void)
{
	if(m_process)
	{
		if(m_process->state() != QProcess::NotRunning)
		{
			return;
		}
	}

	ui->textBrowser->clear();
	m_floatingLabel->show();
}

void CMainWindow::outputAvailable(void)
{
	if(m_process)
	{
		while(m_process->canReadLine())
		{
			QByteArray line = m_process->readLine();
			ui->textBrowser->append(QString::fromUtf8(line).trimmed());
		}
	}
}

void CMainWindow::processStarted(void)
{
	ui->textBrowser->clear();
}

void CMainWindow::processFinished(void)
{
	outputAvailable();
	ui->analyzeButton->setEnabled(true);
	ui->exitButton->setEnabled(true);
	ui->textBrowser->verticalScrollBar()->setValue(0);
	ui->textBrowser->horizontalScrollBar()->setValue(0);
}

void CMainWindow::linkTriggered(void)
{
	QObject *obj = QObject::sender();
	if(QAction *action = dynamic_cast<QAction*>(obj))
	{
		 QDesktopServices::openUrl(QUrl(action->data().toString()));
	}
}

void CMainWindow::showAboutScreen(void)
{
	QString text;

	text += QString().sprintf("<nobr><tt><b>MediaInfoXP v%u.%02u - Simple GUI for MediaInfo</b><br>", mixp_versionMajor, mixp_versionMinor);
	text += QString().sprintf("Copyright (c) 2004-%04d LoRd_MuldeR &lt;mulder2@gmx.de&gt;. Some rights reserved.<br>", qMax(getBuildDate().year(),QDate::currentDate().year()));
	text += QString().sprintf("Built on %s at %s, using Qt Framework v%s.<br><br>", getBuildDate().toString(Qt::ISODate).toLatin1().constData(), mixp_buildTime, qVersion());
	text += QString().sprintf("This program is free software: you can redistribute it and/or modify<br>");
	text += QString().sprintf("it under the terms of the GNU General Public License &lt;http://www.gnu.org/&gt;.<br>");
	text += QString().sprintf("Note that this program is distributed with ABSOLUTELY NO WARRANTY.<br><br>");
	text += QString().sprintf("Please check the web-site at <a href=\"%s\">%s</a> for updates !!!<br>", LINK_MULDER, LINK_MULDER);
	text += QString().sprintf("<hr><br>");
	text += QString().sprintf("This application is powered by MediaInfo v%u.%u.%02u<br>", mixp_miVersionMajor, mixp_miVersionMinor, mixp_miVersionPatch);
	text += QString().sprintf("Free and OpenSource tool for displaying technical information about media files.<br>");
	text += QString().sprintf("Copyright (c) 2002-2013 MediaArea.net SARL. All rights reserved.<br><br>");
	text += QString().sprintf("Redistribution and use is permitted according to the (2-Clause) BSD License.<br>");
	text += QString().sprintf("Please see <a href=\"%s\">%s</a> for more information.<br></tt></nobr>", LINK_MEDIAINFO, LINK_MEDIAINFO);

	QMessageBox aboutBox(this);
	aboutBox.setIconPixmap(QIcon(":/res/logo.png").pixmap(64,64));
	aboutBox.setWindowTitle(tr("About..."));
	aboutBox.setText(text.replace("-", "&minus;"));
	if(QPushButton *btn = aboutBox.addButton(tr("About Qt"), QMessageBox::NoRole))
	{
		btn->setIcon(QIcon(":/res/ico_qt.png"));
		btn->setMinimumWidth(120);
	}
	if(QPushButton *btn = aboutBox.addButton(tr("Discard"), QMessageBox::NoRole))
	{
		btn->setIcon(QIcon(":/res/ico_discard.png"));
		btn->setMinimumWidth(104);
		aboutBox.setEscapeButton(btn);
	}

	forever
	{
		const int ret = aboutBox.exec();
		if(ret == 0)
		{
			QMessageBox::aboutQt(this);
			continue;
		}
		break;
	}
}

void CMainWindow::handleDroppedFile(void)
{
	if(!m_droppedFile.isEmpty())
	{
		analyzeFile(m_droppedFile);
		m_droppedFile.clear();
	}
}

////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////

QDate CMainWindow::getBuildDate(void)
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

#define VALIDATE_MEDIAINFO(HANDLE) do \
{ \
	if(HANDLE != INVALID_HANDLE_VALUE) \
	{ \
		QByteArray buffer(mediaInfoRes.size(), '\0'); DWORD bytesRead = 0; \
		if(GetFileSize(HANDLE, NULL) == mediaInfoRes.size()) \
		{ \
			SetFilePointer(HANDLE, 0L, NULL, FILE_BEGIN); \
			ReadFile(HANDLE, buffer.data(), mediaInfoRes.size(), &bytesRead, NULL); \
		} \
		if(memcmp(buffer.constData(), mediaInfoRes.data(), mediaInfoRes.size()) != 0) \
		{ \
			qWarning("MediaInfo binary failed to validate!"); \
			m_mediaInfoPath.clear(); \
		} \
	} \
} \
while(0)

QString CMainWindow::getMediaInfoPath(void)
{
	QResource mediaInfoRes(":/res/MediaInfo.i386.exe");
	if(!mediaInfoRes.isValid())
	{
		qFatal("MediaInfo resource could not be initialized!");
		return QString();
	}
	
	//Already existsing?
	if(!m_mediaInfoPath.isEmpty())
	{
		QFileInfo mediaInfoNfo(m_mediaInfoPath);
		if(!(mediaInfoNfo.exists() && mediaInfoNfo.isFile()))
		{
			qWarning("MediaInfo binary does NOT seem to exist any longer!\n");
			m_mediaInfoPath.clear();
		}
	}

	//Validate file content
	VALIDATE_MEDIAINFO(m_mediaInfoHandle);

	//Extract MediaInfo
	if(m_mediaInfoPath.isEmpty())
	{
		qDebug("MediaInfo binary not existing yet, going to extract now...\n");
		if(m_mediaInfoHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_mediaInfoHandle);
			m_mediaInfoHandle = INVALID_HANDLE_VALUE;
		}
		QString path = QString("%1/MediaInfo.exe").arg(m_tempFolder);
		QFile mediaInfoFile(path);
		if(mediaInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			if(mediaInfoFile.write(reinterpret_cast<const char*>(mediaInfoRes.data()), mediaInfoRes.size()) == mediaInfoRes.size())
			{
				m_mediaInfoPath = path;
				qDebug("MediaInfo path is:\n%s\n", m_mediaInfoPath.toUtf8().constData());
			}
			else
			{
				qWarning("Failed to write data to MediaInfo binary file!\n");
			}
			mediaInfoFile.close();
		}
		else
		{
			qWarning("Failed to open MediaInfo binary for writing!\n");
		}
	}

	//Open file for reading
	if((!m_mediaInfoPath.isEmpty()) && (m_mediaInfoHandle == INVALID_HANDLE_VALUE))
	{
		m_mediaInfoHandle = CreateFileW(QWCHAR(QDir::toNativeSeparators(m_mediaInfoPath)), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
		if(m_mediaInfoHandle == INVALID_HANDLE_VALUE)
		{
			qWarning("Failed to open the MediaInfo binary for reading!\n");
			m_mediaInfoPath.clear();
		}
	}

	//Validate file content
	VALIDATE_MEDIAINFO(m_mediaInfoHandle);

	//Failed?
	if(m_mediaInfoPath.isEmpty())
	{
		QMessageBox::critical(this, tr("Failure"), tr("Error: Failed to extract MediaInfo binary!"), QMessageBox::Ok);
	}

	return m_mediaInfoPath;
}

bool CMainWindow::analyzeFile(const QString &filePath)
{
	//Create process, if not done yet
	if(!m_process)
	{
		m_process = new QProcess();
		m_process->setProcessChannelMode(QProcess::MergedChannels);
		m_process->setReadChannel(QProcess::StandardOutput);
		connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(outputAvailable()));
		connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(outputAvailable()));
		connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished()));
		connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processFinished()));
		connect(m_process, SIGNAL(started()), this, SLOT(processStarted()));
	}

	//Still running?
	if(m_process->state() != QProcess::NotRunning)
	{
		qWarning("Process is still running!\n");
		return false;
	}

	//Lookup MediaInfo path
	const QString mediaInfoPath = getMediaInfoPath();
	if(mediaInfoPath.isEmpty())
	{
		return false;
	}

	qDebug("Analyzing media file:\n%s\n", filePath.toUtf8().constData());
	m_process->start(mediaInfoPath, QStringList() << QDir::toNativeSeparators(filePath));

	m_floatingLabel->hide();
	ui->analyzeButton->setEnabled(true);
	ui->exitButton->setEnabled(true);

	return true;
}
