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
#include <QClipboard>

//CRT
#include <ctime>

//Win32
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>

//Internal
#include "Config.h"
#include "Utils.h"

//Macros
#define SET_FONT_BOLD(WIDGET,BOLD) { QFont _font = WIDGET->font(); _font.setBold(BOLD); WIDGET->setFont(_font); }
#define SET_TEXT_COLOR(WIDGET,COLOR) { QPalette _palette = WIDGET->palette(); _palette.setColor(QPalette::WindowText, (COLOR)); _palette.setColor(QPalette::Text, (COLOR)); WIDGET->setPalette(_palette); }

//Text
const char *STATUS_BLNK = ">> You can drop any type of media files here <<";
const char *STATUS_WORK = "Analyzing file(s), this may take a moment or two...<br><br><br><br><img src=\":/res/loading.png\">";

//Links
const char *LINK_MULDER = "http://muldersoft.com/";
const char *LINK_MEDIAINFO = "http://mediaarea.net/en/MediaInfo"; /*"http://mediainfo.sourceforge.net/en"*/
const char *LINK_DISCUSS = "http://forum.doom9.org/showthread.php?t=96516";

//HTML characters
static QList<QPair<const QString, const QString>> HTML_ESCAPE(void)
{
	QList<QPair<const QString, const QString>> htmlEscape;
	htmlEscape << QPair<const QString, const QString>("<", "&lt;");
	htmlEscape << QPair<const QString, const QString>(">", "&gt;");
	htmlEscape << QPair<const QString, const QString>("&", "&amp;");
	return htmlEscape;
}

////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////

CMainWindow::CMainWindow(const QString &tempFolder, QWidget *parent)
:
	QMainWindow(parent),
	m_tempFolder(tempFolder),
	m_firstShow(true),
	m_htmlEscape(HTML_ESCAPE()),
	ui(new Ui::MainWindow)
{
	//Init UI
	qApp->setStyle(QStyleFactory::create("plastique"));
	ui->setupUi(this);
	setMinimumSize(this->size());

	//Setup Icon
	m_icon = mixp_set_window_icon(this, QIcon(":/res/logo.png"), true);

	//Setup links
	ui->actionLink_MuldeR->setData(QVariant(QString::fromLatin1(LINK_MULDER)));
	ui->actionLink_MediaInfo->setData(QVariant(QString::fromLatin1(LINK_MEDIAINFO)));
	ui->actionLink_Discuss->setData(QVariant(QString::fromLatin1(LINK_DISCUSS)));

	//Setup connections
	connect(ui->analyzeButton, SIGNAL(clicked()), this, SLOT(analyzeButtonClicked()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(analyzeButtonClicked()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveButtonClicked()));
	connect(ui->actionCopyToClipboard, SIGNAL(triggered()), this, SLOT(copyToClipboardButtonClicked()));
	connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clearButtonClicked()));
	connect(ui->actionLink_MuldeR, SIGNAL(triggered()), this, SLOT(linkTriggered()));
	connect(ui->actionLink_MediaInfo, SIGNAL(triggered()), this, SLOT(linkTriggered()));
	connect(ui->actionLink_Discuss, SIGNAL(triggered()), this, SLOT(linkTriggered()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutScreen()));
	ui->versionLabel->installEventFilter(this);

	//Context menu
	ui->textBrowser->setContextMenuPolicy(Qt::ActionsContextMenu);
	ui->textBrowser->insertActions(0, ui->menuFile->actions());

	//Create label
	m_floatingLabel = new QLabel(ui->textBrowser);
	m_floatingLabel->setText(QString::fromLatin1(STATUS_BLNK));
	m_floatingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_floatingLabel->show();
	SET_TEXT_COLOR(m_floatingLabel, Qt::darkGray);
	SET_FONT_BOLD(m_floatingLabel, true);
	m_floatingLabel->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_floatingLabel->insertActions(0, ui->textBrowser->actions());
	
	//Clear
	m_mediaInfoHandle = NULL;
	m_process = NULL;

	//Randomize
	qsrand((uint) time(NULL));
}

////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////

CMainWindow::~CMainWindow(void)
{
	if(m_mediaInfoHandle != NULL)
	{
		m_mediaInfoHandle->remove();
		MIXP_DELETE_OBJ(m_mediaInfoHandle);
	}
	MIXP_DELETE_OBJ(m_process);
	MIXP_DELETE_OBJ(m_floatingLabel);

	mixp_free_window_icon(m_icon);
}

////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////

void CMainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
	resize(this->minimumSize());
	
	//Init test
	ui->versionLabel->setText(QString("v%1 / v%2 (%3)").arg(QString().sprintf("%u.%02u", mixp_versionMajor, mixp_versionMinor), QString().sprintf("%u.%u.%02u", mixp_miVersionMajor, mixp_miVersionMinor, mixp_miVersionPatch), mixp_get_build_date().toString(Qt::ISODate)));
	ui->updateLabel->setText(tr("This version is more than six month old and probably outdated. Please check <a href=\"%1\">%1</a> for updates!").arg(LINK_MULDER));

	//Show update hint?
	ui->updateLabel->setVisible(mixp_get_build_date().addMonths(6) < mixp_get_current_date());

	//Force resize event
	resizeEvent(NULL);
	QTimer::singleShot(0, this, SLOT(updateSize()));

	//Enable drag & drop support
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
						m_pendingFiles << currentFile.canonicalFilePath();
					}
					continue;
				}
				break;
			}
		}
		if(!m_pendingFiles.empty())
		{
			QTimer::singleShot(0, this, SLOT(analyzeFiles()));
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
	updateSize();
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
	if(m_process && (m_process->state() != QProcess::NotRunning))
	{
		qWarning("Process is still running!\n");
		return;
	}

	m_pendingFiles.clear();
	QList<QUrl> urls = event->mimeData()->urls();

	while(!urls.isEmpty())
	{
		QUrl currentUrl = urls.takeFirst();
		QFileInfo file(currentUrl.toLocalFile());
		if(file.exists() && file.isFile())
		{
			m_pendingFiles << file.canonicalFilePath();
		}
	}

	if(!m_pendingFiles.isEmpty())
	{
		QTimer::singleShot(0, this, SLOT(analyzeFiles()));
	}
}

void CMainWindow::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Escape)
	{
		if(m_process && (m_process->state() != QProcess::NotRunning))
		{
			mixp_beep(mixp_beep_error);
			qWarning("Escape pressed, terminated process!");
			m_process->kill();
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

void CMainWindow::analyzeFiles(void)
{
	//Any files pending?
	if(m_pendingFiles.isEmpty())
	{
		qDebug("No pending files, nothing to do!\n");
		return;
	}
	
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
	}

	//Still running?
	if(m_process->state() != QProcess::NotRunning)
	{
		qWarning("Process is still running!\n");
		return;
	}

	//Clear data
	ui->textBrowser->clear();
	m_outputLines.clear();

	//Disable buttons
	ui->analyzeButton->setEnabled(false);
	ui->exitButton->setEnabled(false);
	ui->actionClear->setEnabled(false);
	ui->actionCopyToClipboard->setEnabled(false);
	ui->actionSave->setEnabled(false);
	ui->actionOpen->setEnabled(false);

	//Show banner
	m_floatingLabel->show();
	m_floatingLabel->setText(QString::fromLatin1(STATUS_WORK));
	m_floatingLabel->setCursor(Qt::WaitCursor);
	
	//Trigger GUI update
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

	//Give it a go!
	QTimer::singleShot(0, this, SLOT(analyzeNextFile()));
}

void CMainWindow::analyzeNextFile(void)
{
	if(m_pendingFiles.isEmpty())
	{
		qWarning("Oups, no pending files!");
		return;
	}

	//Lookup MediaInfo path
	const QString mediaInfoPath = getMediaInfoPath();
	if(mediaInfoPath.isEmpty())
	{
		ui->textBrowser->setHtml(QString("<pre>%1</pre>").arg(tr("Oups, failed to extract MediaInfo binary!")));
		QMessageBox::critical(this, tr("Failure"), tr("Error: Failed to extract MediaInfo binary!"), QMessageBox::Ok);
		m_floatingLabel->hide();
		ui->actionOpen->setEnabled(true);
		ui->analyzeButton->setEnabled(true);
		ui->exitButton->setEnabled(true);
		return;
	}

	const QString filePath = m_pendingFiles.takeFirst();

	//Start analyziation
	qDebug("Analyzing media file:\n%s\n", filePath.toUtf8().constData());
	m_process->start(mediaInfoPath, QStringList() << QDir::toNativeSeparators(filePath));

	//Wait for process to start
	if(!m_process->waitForStarted())
	{
		qWarning("Process failed to start:\n%s\n", m_process->errorString().toLatin1().constData());
		ui->textBrowser->setHtml(QString("<pre>%1</pre>").arg(tr("Oups, failed to create MediaInfo process!")));
		QMessageBox::critical(this, tr("Failure"), tr("Error: Failed to create MediaInfo process!"), QMessageBox::Ok);
		m_floatingLabel->hide();
		ui->actionOpen->setEnabled(true);
		ui->analyzeButton->setEnabled(true);
		ui->exitButton->setEnabled(true);
		return;
	}

	qDebug("Process started successfully (PID: %u)", 42); //m_process->pid()->dwProcessId);
}

void CMainWindow::analyzeButtonClicked(void)
{
	if(m_process && (m_process->state() != QProcess::NotRunning))
	{
		qWarning("Process is still running!\n");
		return;
	}

	const QStringList selectedFiles = QFileDialog::getOpenFileNames(this, tr("Select file to analyze..."), QString(), tr("All supported media files (*.*)"));
	if(!selectedFiles.isEmpty())
	{
		m_pendingFiles.clear();
		m_pendingFiles << selectedFiles;
		analyzeFiles();
	}
}

void CMainWindow::saveButtonClicked(void)
{
	if(m_process && (m_process->state() != QProcess::NotRunning))
	{
		qWarning("Process is still running!\n");
		return;
	}
		
	const QString selectedFile = QFileDialog::getSaveFileName(this, tr("Select file to save..."), QString(), tr("Plain Text (*.txt)"));
	if(!selectedFile.isEmpty())
	{
		QFile file(selectedFile);
		if(file.open(QIODevice::WriteOnly | QIODevice::WriteOnly))
		{
			file.write(m_outputLines.join("\r\n").toUtf8());
			file.close();
			mixp_beep(mixp_beep_info);
		}
		else
		{
			QMessageBox::critical(this, tr("Failure"), tr("Error: Failed to open the file writing!"), QMessageBox::Ok);
		}
	}
}

void CMainWindow::copyToClipboardButtonClicked(void)
{
	if(m_process && (m_process->state() != QProcess::NotRunning))
	{
		qWarning("Process is still running!\n");
		return;
	}

	if(QClipboard *clipboard = QApplication::clipboard())
	{
		clipboard->setText(m_outputLines.join("\n"));
		mixp_beep(mixp_beep_info);
	}
}

void CMainWindow::clearButtonClicked(void)
{
	if(m_process && (m_process->state() != QProcess::NotRunning))
	{
		qWarning("Process is still running!\n");
		return;
	}

	//Clear data and re-show banner
	ui->textBrowser->clear();
	m_floatingLabel->setText(QString::fromLatin1(STATUS_BLNK));
	m_floatingLabel->setCursor(Qt::ArrowCursor);
	m_floatingLabel->show();

	//Disable actions
	ui->actionClear->setEnabled(false);
	ui->actionCopyToClipboard->setEnabled(false);
	ui->actionSave->setEnabled(false);
}

void CMainWindow::outputAvailable(void)
{
	if(m_process)
	{
		//Update lines
		while(m_process->canReadLine())
		{
			QString line = QString::fromUtf8(m_process->readLine()).trimmed();
			if(!(line.isEmpty() && m_outputLines.empty()))
			{
				m_outputLines << line;
			}
		}
	}
}

void CMainWindow::processFinished(void)
{
	//Fetch any remaining data
	outputAvailable();
	
	//Remove trailing blank lines
	while((!m_outputLines.isEmpty()) && m_outputLines.last().trimmed().isEmpty())
	{
		m_outputLines.takeLast();
	}

	//Failed?
	if(m_outputLines.empty())
	{
		m_outputLines << tr("Oups, apparently MediaInfo encountered a problem!");
	}

	//Append one last linebreak
	m_outputLines << "";

	//Scroll up
	ui->textBrowser->verticalScrollBar()->setValue(0);
	ui->textBrowser->horizontalScrollBar()->setValue(0);

	//Check exit code
	const int exitCode = m_process->exitCode();
	qDebug("Process has finished (Code: %d)\n", exitCode);

	//More files left?
	if(!m_pendingFiles.isEmpty() && (exitCode == 0))
	{
		if(!m_outputLines.empty())
		{
			m_outputLines << QString().fill('-', 104) << "";
		}
		QTimer::singleShot(0, this, SLOT(analyzeNextFile()));
		return;
	}

	//Hide banner
	if(m_floatingLabel->isVisible()) m_floatingLabel->hide();

	//Convert to HTML
	QStringList htmlData(m_outputLines);
	escapeHtmlChars(htmlData);

	//Highlight headers
	htmlData.replaceInStrings(QRegExp("^(-+)$"), "<font color=\"darkgray\">\\1</font>");				//Separator lines
	htmlData.replaceInStrings(QRegExp("^([^:<>]+):(.+)$"), "<font color=\"darkblue\">\\1:</font>\\2");	//Info lines
	htmlData.replaceInStrings(QRegExp("^([^:<>]+)$"), "<b><font color=\"darkred\">\\1</font></b>");		//Heading lines

	//Update document
	ui->textBrowser->setHtml(QString("<pre>%1</pre>").arg(htmlData.join("<br>")));

	//Enable actions
	if(!m_outputLines.empty())
	{
		ui->actionClear->setEnabled(true);
		ui->actionCopyToClipboard->setEnabled(true);
		ui->actionSave->setEnabled(true);
	}
	ui->actionOpen->setEnabled(true);
	ui->analyzeButton->setEnabled(true);
	ui->exitButton->setEnabled(true);
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
	if(m_process && (m_process->state() != QProcess::NotRunning))
	{
		qWarning("Process is still running!\n");
		return;
	}

	const QDate buildDate = mixp_get_build_date();
	const QDate curntDate = mixp_get_current_date();

	QString text;

	text += QString().sprintf("<nobr><tt><b>MediaInfoXP v%u.%02u - Simple GUI for MediaInfo</b><br>", mixp_versionMajor, mixp_versionMinor);
	text += QString().sprintf("Copyright (c) 2004-%04d LoRd_MuldeR &lt;mulder2@gmx.de&gt;. Some rights reserved.<br>", qMax(buildDate.year(),curntDate.year()));
	text += QString().sprintf("Built on %s at %s, using Qt Framework v%s.<br><br>", buildDate.toString(Qt::ISODate).toLatin1().constData(), mixp_buildTime, qVersion());
	text += QString().sprintf("This program is free software: you can redistribute it and/or modify<br>");
	text += QString().sprintf("it under the terms of the GNU General Public License &lt;http://www.gnu.org/&gt;.<br>");
	text += QString().sprintf("Note that this program is distributed with ABSOLUTELY NO WARRANTY.<br><br>");
	text += QString().sprintf("Please check the web-site at <a href=\"%s\">%s</a> for updates !!!<br>", LINK_MULDER, LINK_MULDER);
	text += QString().sprintf("<hr><br>");
	text += QString().sprintf("This application is powered by MediaInfo v%u.%u.%02u<br>", mixp_miVersionMajor, mixp_miVersionMinor, mixp_miVersionPatch);
	text += QString().sprintf("Free and OpenSource tool for displaying technical information about media files.<br>");
	text += QString().sprintf("Copyright (c) 2002-%04d MediaArea.net SARL. All rights reserved.<br><br>", qMax(buildDate.year(),curntDate.year()));
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

void CMainWindow::updateSize(void)
{
	if(const QWidget *const viewPort = ui->textBrowser->viewport())
	{
		m_floatingLabel->setGeometry(viewPort->x(), viewPort->y(), viewPort->width(), viewPort->height());
	}
}

////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////

#define VALIDATE_MEDIAINFO(HANDLE) do \
{ \
	if((HANDLE)) \
	{ \
		(HANDLE)->seek(0); \
		QByteArray buffer = (HANDLE)->readAll(); \
		if((buffer.size() != mediaInfoRes.size()) || (memcmp(buffer.constData(), mediaInfoRes.data(), mediaInfoRes.size()) != 0)) \
		{ \
			qWarning("MediaInfo binary failed to validate!"); \
			(HANDLE)->remove(); \
			MIXP_DELETE_OBJ((HANDLE)); \
		} \
	} \
} \
while(0)

QString CMainWindow::getMediaInfoPath(void)
{
	QResource mediaInfoRes(":/res/MediaInfo.i386.exe");
	if((!mediaInfoRes.isValid()) || (!mediaInfoRes.data()))
	{
		qFatal("MediaInfo resource could not be initialized!");
		return QString();
	}
	
	//Validate file content
	VALIDATE_MEDIAINFO(m_mediaInfoHandle);

	//Extract MediaInfo
	if(!m_mediaInfoHandle)
	{
		qDebug("MediaInfo binary not existing yet, going to extract now...\n");
		m_mediaInfoHandle = new QFile(QString("%1/MediaInfo_%2.exe").arg(m_tempFolder, QString().sprintf("%04x", qrand() % 0xFFFF)));
		if(m_mediaInfoHandle->open(QIODevice::ReadWrite | QIODevice::Truncate))
		{
			if(m_mediaInfoHandle->write(reinterpret_cast<const char*>(mediaInfoRes.data()), mediaInfoRes.size()) == mediaInfoRes.size())
			{
				qDebug("MediaInfo path is:\n%s\n", m_mediaInfoHandle->fileName().toUtf8().constData());
				m_mediaInfoHandle->close();
				if(!m_mediaInfoHandle->open(QIODevice::ReadOnly))
				{
					qWarning("Failed to open MediaInfo binary for reading!\n");
					m_mediaInfoHandle->remove();
					MIXP_DELETE_OBJ(m_mediaInfoHandle);
				}
			}
			else
			{
				qWarning("Failed to write data to MediaInfo binary file!\n");
				m_mediaInfoHandle->remove();
				MIXP_DELETE_OBJ(m_mediaInfoHandle);
			}
		}
		else
		{
			qWarning("Failed to open MediaInfo binary for writing!\n");
			MIXP_DELETE_OBJ(m_mediaInfoHandle);
		}
	}

	//Validate file content
	VALIDATE_MEDIAINFO(m_mediaInfoHandle);

	//Return current MediaInfo path
	return m_mediaInfoHandle ? m_mediaInfoHandle->fileName() : QString();
}

void CMainWindow::escapeHtmlChars(QStringList &strings)
{
	QList<QPair<const QString, const QString>>::ConstIterator iter;
	for(iter = m_htmlEscape.constBegin(); iter != m_htmlEscape.constEnd(); iter++)
	{
		strings.replaceInStrings((*iter).first, (*iter).second);
	}
}
