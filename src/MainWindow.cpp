///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2018 LoRd_MuldeR <MuldeR2@GMX.de>
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
#include "UIC_Dialog.h"

//MUtils
#include <MUtils/GUI.h>
#include <MUtils/OSSupport.h>
#include <MUtils/CPUFeatures.h>
#include <MUtils/Sound.h>
#include <MUtils/Hash.h>
#include <MUtils/Exception.h>
#include <MUtils/Version.h>

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
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

//CRT
#include <ctime>

//Internal
#include "Config.h"
#include "ShellExtension.h"
#include "IPC.h"

//Macros
#define MIXP_VERSION_STR (QString().sprintf("%u.%02u", g_mixp_versionMajor, g_mixp_versionMinor, g_mixp_versionMinor))
#define MI_VERSION_STR (QString().sprintf((g_mixp_mediaInfoVerPatch > 0) ? "%u.%02u.%u" : "%u.%02u", g_mixp_mediaInfoVerMajor, g_mixp_mediaInfoVerMinor, g_mixp_mediaInfoVerPatch))
#define UTIL_VERSION_STR (QString().sprintf("%u.%02u", MUtils::Version::lib_version_major(), MUtils::Version::lib_version_minor()))
#define SET_FONT_BOLD(WIDGET,BOLD) { QFont _font = WIDGET->font(); _font.setBold(BOLD); WIDGET->setFont(_font); }
#define SET_TEXT_COLOR(WIDGET,COLOR) { QPalette _palette = WIDGET->palette(); _palette.setColor(QPalette::WindowText, (COLOR)); _palette.setColor(QPalette::Text, (COLOR)); WIDGET->setPalette(_palette); }
#define APPLICATION_IS_IDLE (m_status == APP_STATUS_IDLE)

//Text
const char *STATUS_BLNK = ">> You can drop any type of media files here <<";
const char *STATUS_WORK = "Analyzing file(s), this may take a moment or two...<br><br><br><br><img src=\":/res/loading.png\">";

//Links
const char *LINK_MULDER    = "http://muldersoft.com/";
const char *LINK_MEDIAINFO = "http://mediaarea.net/en/MediaInfo"; /*"http://mediainfo.sourceforge.net/en"*/
const char *LINK_DISCUSS   = "http://forum.doom9.org/showthread.php?t=96516";

//MediaInfo
static const struct
{
	const char *const arch;
	const char *const checksum;
	const bool require_x64;
	const bool require_sse2;
}
MEDIAINFO_BIN[] =
{
	{ "x64-sse2", g_mixp_checksum_x64, bool(1), bool(1) },
	{ "x86-sse2", g_mixp_checksum_sse, bool(0), bool(1) },
	{ "x86-i686", g_mixp_checksum_gen, bool(0), bool(0) },
	{ NULL, NULL, false, false }
};

//Const
static const int FILE_RECEIVE_DELAY = 1750;

////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////

CMainWindow::CMainWindow(const QString &tempFolder, MUtils::IPCChannel *const ipc, QWidget *parent)
:
	QMainWindow(parent),
	m_tempFolder(tempFolder),
	m_ipcThread(new IPCReceiveThread(ipc)),
	m_status(APP_STATUS_STARTING),
	ui(new Ui::MainWindow)
{
	//Init UI
	qApp->setStyle(QStyleFactory::create("plastique"));
	ui->setupUi(this);
	setMinimumSize(this->size());

	//Setup Icon
	MUtils::GUI::set_window_icon(this, QIcon(":/res/logo.png"), true);

	//Adjust size to DPI settings and re-center
	MUtils::GUI::scale_widget(this);

	//Setup links
	ui->actionLink_MuldeR   ->setData(QVariant(QString::fromLatin1(LINK_MULDER)));
	ui->actionLink_MediaInfo->setData(QVariant(QString::fromLatin1(LINK_MEDIAINFO)));
	ui->actionLink_Discuss  ->setData(QVariant(QString::fromLatin1(LINK_DISCUSS)));

	//Setup connections
	connect(ui->analyzeButton,         SIGNAL(clicked()),                  this, SLOT(analyzeButtonClicked()));
	connect(ui->actionOpen,            SIGNAL(triggered()),                this, SLOT(analyzeButtonClicked()));
	connect(ui->actionSave,            SIGNAL(triggered()),                this, SLOT(saveButtonClicked()));
	connect(ui->actionCopyToClipboard, SIGNAL(triggered()),                this, SLOT(copyToClipboardButtonClicked()));
	connect(ui->actionClear,           SIGNAL(triggered()),                this, SLOT(clearButtonClicked()));
	connect(ui->actionLink_MuldeR,     SIGNAL(triggered()),                this, SLOT(linkTriggered()));
	connect(ui->actionLink_MediaInfo,  SIGNAL(triggered()),                this, SLOT(linkTriggered()));
	connect(ui->actionLink_Discuss,    SIGNAL(triggered()),                this, SLOT(linkTriggered()));
	connect(ui->actionAbout,           SIGNAL(triggered()),                this, SLOT(showAboutScreen()));
	connect(ui->actionShellExtension,  SIGNAL(toggled(bool)),              this, SLOT(updateShellExtension(bool)));
	connect(ui->actionLineWrapping,    SIGNAL(toggled(bool)),              this, SLOT(updateLineWrapping(bool)));
	connect(ui->actionVerboseOutput,   SIGNAL(toggled(bool)),              this, SLOT(toggleOutputOption(bool)));
	connect(ui->actionXmlOutput,       SIGNAL(toggled(bool)),              this, SLOT(toggleOutputOption(bool)));
	connect(m_ipcThread.data(),        SIGNAL(received(quint32, QString)), this, SLOT(received(quint32, QString)));
	ui->versionLabel->installEventFilter(this);

	//Context menu
	ui->textBrowser->setContextMenuPolicy(Qt::ActionsContextMenu);
	ui->textBrowser->insertActions(0, ui->menuFile->actions());

	//Create label
	m_floatingLabel.reset(new QLabel(ui->textBrowser));
	m_floatingLabel->setText(QString::fromLatin1(STATUS_BLNK));
	m_floatingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_floatingLabel->show();
	SET_TEXT_COLOR(m_floatingLabel, Qt::darkGray);
	SET_FONT_BOLD(m_floatingLabel, true);
	m_floatingLabel->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_floatingLabel->insertActions(0, ui->textBrowser->actions());

	//Randomize
	qsrand((uint) time(NULL));

	//Selftest
	if (MUTILS_DEBUG)
	{
		selfTest();
	}
}

////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////

CMainWindow::~CMainWindow(void)
{
	if(!m_ipcThread.isNull())
	{
		m_ipcThread->stop();
		if(!m_ipcThread->wait(5000))
		{
			qWarning("IPC thread doesn't respond -> terminating!");
			m_ipcThread->terminate();
			m_ipcThread->wait();
		}
	}

	if(!m_mediaInfoHandle.isNull())
	{
		m_mediaInfoHandle->remove();
	}
}

////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////

void CMainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
	
	//Init test
	ui->versionLabel->setText(QString("v%1 / v%2 (%3)").arg(MIXP_VERSION_STR, MI_VERSION_STR, MUtils::Version::app_build_date().toString(Qt::ISODate)));
	ui->updateLabel->setText(tr("This version is more than six month old and probably outdated. Please check <a href=\"%1\">%1</a> for updates!").arg(LINK_MULDER));

	//Show update hint?
	ui->updateLabel->setVisible(MUtils::Version::app_build_date().addMonths(6) < MUtils::OS::current_date());

	//Force resize event
	resizeEvent(NULL);
	QTimer::singleShot(0, this, SLOT(updateSize()));

	if(m_status == APP_STATUS_STARTING)
	{
		const MUtils::OS::ArgumentMap arguments = MUtils::OS::arguments();
		const QStringList files = arguments.values("open");
		for(QStringList::ConstIterator iter = files.constBegin(); iter != files.constEnd(); iter++)
		{
			QFileInfo currentFile = QFileInfo(*iter);
			if(currentFile.exists() && currentFile.isFile())
			{
				m_pendingFiles << currentFile.canonicalFilePath();
			}
		}
		if(!m_pendingFiles.empty())
		{
			m_status = APP_STATUS_AWAITING;
			QTimer::singleShot(FILE_RECEIVE_DELAY, this, SLOT(analyzeFiles()));
		}

		QTimer::singleShot(250, this, SLOT(initShellExtension()));
		m_ipcThread->start();

		if(m_status == APP_STATUS_STARTING)
		{
			m_status = APP_STATUS_IDLE;
		}
	}

	//Enable drag & drop support
	setAcceptDrops(true);
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot exit program at this time!");
		event->ignore();
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
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot process files at this time!\n");
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
		m_status = APP_STATUS_WORKING;
		QTimer::singleShot(0, this, SLOT(analyzeFiles()));
	}
}

void CMainWindow::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Escape)
	{
		if(m_process && (m_process->state() != QProcess::NotRunning))
		{
			MUtils::Sound::beep(MUtils::Sound::BEEP_ERR);
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
	if(m_process.isNull())
	{
		m_process.reset(new QProcess());
		m_process->setProcessChannelMode(QProcess::MergedChannels);
		m_process->setReadChannel(QProcess::StandardOutput);
		connect(m_process.data(), SIGNAL(readyReadStandardError()),           this, SLOT(outputAvailable()));
		connect(m_process.data(), SIGNAL(readyReadStandardOutput()),          this, SLOT(outputAvailable()));
		connect(m_process.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished()));
		connect(m_process.data(), SIGNAL(error(QProcess::ProcessError)),      this, SLOT(processFinished()));
	}

	//Still running?
	if(m_process->state() != QProcess::NotRunning)
	{
		qWarning("Process is still running!\n");
		return;
	}

	m_status = APP_STATUS_WORKING;

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
	ui->menuPreferences->setEnabled(false);

	//Show banner
	m_floatingLabel->show();
	m_floatingLabel->setText(QString::fromLatin1(STATUS_WORK));
	m_floatingLabel->setCursor(Qt::WaitCursor);
		
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

	//Still running?
	if(m_process->state() != QProcess::NotRunning)
	{
		qWarning("Process is still running!\n");
		return;
	}

	m_status = APP_STATUS_WORKING;

	//Lookup MediaInfo path
	const QString mediaInfoPath = getMediaInfoPath();
	if(mediaInfoPath.isEmpty())
	{
		ui->textBrowser->setHtml(QString("<pre>%1</pre>").arg(tr("Oups, failed to extract MediaInfo binary!")));
		QMessageBox::critical(this, tr("Failure"), tr("Fatal Error: Failed to extract the MediaInfo binary!"), QMessageBox::Ok);
		m_floatingLabel->hide();
		ui->actionOpen->setEnabled(true);
		ui->analyzeButton->setEnabled(true);
		ui->exitButton->setEnabled(true);
		ui->menuPreferences->setEnabled(true);
		m_status = APP_STATUS_IDLE;
		return;
	}

	const QString filePath = m_pendingFiles.takeFirst();

	//Generate the command line
	QStringList commandLine;
	if(ui->actionVerboseOutput->isChecked()) commandLine << "--Full";
	if(ui->actionXmlOutput->isChecked()) commandLine << "--Output=XML";
	commandLine << QDir::toNativeSeparators(filePath);

	//Start analyziation
	qDebug("Analyzing media file:\n%s\n", filePath.toUtf8().constData());
	m_process->start(mediaInfoPath, commandLine);

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
		ui->menuPreferences->setEnabled(true);
		m_status = APP_STATUS_IDLE;
		return;
	}

	qDebug("Process started successfully (PID: %u)", MUtils::OS::process_id(m_process.data()));
}

void CMainWindow::analyzeButtonClicked(void)
{
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot open files at this time!\n");
		return;
	}

	m_status = APP_STATUS_BLOCKED;
	const QStringList selectedFiles = QFileDialog::getOpenFileNames(this, tr("Select file to analyze..."), QString(), tr("All supported media files (*.*)"));
	m_status = APP_STATUS_IDLE;

	if(!selectedFiles.isEmpty())
	{
		m_pendingFiles.clear();
		m_pendingFiles << selectedFiles;
		m_status = APP_STATUS_WORKING;
		analyzeFiles();
	}
}

void CMainWindow::saveButtonClicked(void)
{
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot process files at this time!\n");
		return;
	}

	m_status = APP_STATUS_BLOCKED;
	const QString selectedFile = QFileDialog::getSaveFileName(this, tr("Select file to save..."), QString(), tr("Plain Text (*.txt)"));
	m_status = APP_STATUS_IDLE;

	if(!selectedFile.isEmpty())
	{
		QFile file(selectedFile);
		if(file.open(QIODevice::WriteOnly | QIODevice::WriteOnly))
		{
			file.write(m_outputLines.join("\r\n").toUtf8());
			file.close();
			MUtils::Sound::beep(MUtils::Sound::BEEP_NFO);
		}
		else
		{
			QMessageBox::critical(this, tr("Failure"), tr("Error: Failed to open the file writing!"), QMessageBox::Ok);
		}
	}
}

void CMainWindow::copyToClipboardButtonClicked(void)
{
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot process files at this time!\n");
		return;
	}

	if(QClipboard *clipboard = QApplication::clipboard())
	{
		clipboard->setText(m_outputLines.join("\n"));
		MUtils::Sound::beep(MUtils::Sound::BEEP_NFO);
	}
}

void CMainWindow::clearButtonClicked(void)
{
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot process files at this time!\n");
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

	//Remove leading "E:" lines
	/*while ((!m_outputLines.isEmpty()) && m_outputLines.first().trimmed().startsWith(QLatin1String("E: ")))
	//{
	//	qWarning("E: line has been removed!");
	//	m_outputLines.takeFirst();
	}*/

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

	//Update document
	if (!ui->actionXmlOutput->isChecked())
	{
		QStringList htmlData = escapeHtmlLines(m_outputLines);
		htmlData.replaceInStrings(QRegExp("^(-+)$"), "<font color=\"darkgray\">\\1</font>");
		htmlData.replaceInStrings(QRegExp("^([^:<>]+):(.+)$"), "<font color=\"darkblue\">\\1:</font>\\2");
		htmlData.replaceInStrings(QRegExp("^([^:<>]+)$"), "<b><font color=\"darkred\">\\1</font></b>");
		ui->textBrowser->setHtml(QString("<pre style=\"white-space:pre-wrap\">%1</pre>").arg(htmlData.join("<br>")));
	}
	else
	{
		const QString xmlData = Qt::escape(reformatXml(m_outputLines.join(QLatin1String("\n"))));
		ui->textBrowser->setHtml(QString("<pre style=\"white-space:pre-wrap\">%1</pre>").arg(xmlData));
	}

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
	ui->menuPreferences->setEnabled(true);

	m_status = APP_STATUS_IDLE;
}

void CMainWindow::initShellExtension(void)
{
	const bool isEnabled = ShellExtension::getEnabled();
	
	if(isEnabled)
	{
		ShellExtension::setEnabled(true);
	}

	ui->actionShellExtension->blockSignals(true);
	ui->actionShellExtension->setChecked(isEnabled);
	ui->actionShellExtension->setEnabled(true);
	ui->actionShellExtension->blockSignals(false);
}

void CMainWindow::updateShellExtension(bool checked)
{
	ShellExtension::setEnabled(checked);
}

void CMainWindow::updateLineWrapping(bool checked)
{
	ui->textBrowser->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void CMainWindow::toggleOutputOption(bool checked)
{
	QMessageBox::information(this, this->windowTitle(), tr("Changes will take effect the next time that you open a file!"));
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
	if(!APPLICATION_IS_IDLE)
	{
		qWarning("Cannot show about box at this time!\n");
		return;
	}

	const QDate buildDate = MUtils::Version::app_build_date();
	const QDate curntDate = MUtils::OS::current_date();
	const int year = qMax(buildDate.year(), curntDate.year());

	QString text;

	text += QString("<nobr><tt><b>MediaInfoXP v%1 - Simple GUI for MediaInfo</b><br>").arg(MIXP_VERSION_STR);
	text += QString("Copyright (c) 2004-%1 LoRd_MuldeR &lt;mulder2@gmx.de&gt;. Some rights reserved.<br>").arg(QString().sprintf("%04d", year));
	text += QString("Built on %1 at %2, using Qt Framework v%3.<br>").arg(buildDate.toString(Qt::ISODate), QString::fromLatin1(g_mixp_buildTime), QString::fromLatin1(qVersion()));
	text += QString("Linked against MUtilities library v%1, built on %2 at %3.<br><br>").arg(UTIL_VERSION_STR, MUtils::Version::lib_build_date().toString(Qt::ISODate), MUtils::Version::lib_build_time().toString(Qt::ISODate));
	text += QString("This program is free software: you can redistribute it and/or modify<br>");
	text += QString("it under the terms of the GNU General Public License &lt;http://www.gnu.org/&gt;.<br>");
	text += QString("Note that this program is distributed with ABSOLUTELY NO WARRANTY.<br><br>");
	text += QString("Please check the web-site at <a href=\"%1\">%1</a> for updates !!!<br>").arg(QString::fromLatin1(LINK_MULDER));
	text += QString("<hr><br>");
	text += QString("<b>This application is powered by MediaInfo v%1</b><br>").arg(MI_VERSION_STR);
	text += QString("Free and OpenSource tool for displaying technical information about media files.<br>");
	text += QString("Copyright (c) 2002-%1 MediaArea.net SARL. All rights reserved.<br><br>").arg(QString().sprintf("%04d", year));
	text += QString("Redistribution and use is permitted according to the (2-Clause) BSD License.<br>");
	text += QString("Please see <a href=\"%1\">%1</a> for more information.<br></tt></nobr>").arg(QString::fromLatin1(LINK_MEDIAINFO));

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

	m_status = APP_STATUS_BLOCKED;

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

	m_status = APP_STATUS_IDLE;
}

void CMainWindow::updateSize(void)
{
	if(const QWidget *const viewPort = ui->textBrowser->viewport())
	{
		m_floatingLabel->setGeometry(viewPort->x(), viewPort->y(), viewPort->width(), viewPort->height());
	}
}

void CMainWindow::received(const quint32 &command, const QString &message)
{
	if((command == IPC::COMMAND_PING) || (command == IPC::COMMAND_OPEN))
	{
		MUtils::GUI::bring_to_front(this);
	}

	if((command == IPC::COMMAND_OPEN) && (!message.isEmpty()))
	{
		qDebug("Received file: %s", message.toUtf8().constData());
	
		if((m_status != APP_STATUS_IDLE) && (m_status != APP_STATUS_AWAITING))
		{
			qWarning("Cannot process files at this time!\n");
			return;
		}

		const QString absPath = QFileInfo(QDir::fromNativeSeparators(message)).absoluteFilePath();
		QFileInfo fileInfo(absPath);
		if(fileInfo.exists() && fileInfo.isFile())
		{
			m_pendingFiles << fileInfo.canonicalFilePath();
			if(m_status == APP_STATUS_IDLE)
			{
				m_status = APP_STATUS_AWAITING;
				QTimer::singleShot(FILE_RECEIVE_DELAY, this, SLOT(analyzeFiles()));
			}
		}
	}
}

////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////

#define HAVE_SSE2(X) ((X).features & MUtils::CPUFetaures::FLAG_SSE2)
static const char *const HASH_SEED = "+A`~}vPe9'~#n+c1Wq/MPo;1XwY\\;Pb.";

static bool VALIDATE_MEDIAINFO(QFile *const handle, const char *const expected_checksum)
{
	if(!handle->reset())
	{
		return false;
	}

	//Compute Hash
	QScopedPointer<MUtils::Hash::Hash> hash(MUtils::Hash::create(MUtils::Hash::HASH_BLAKE2_512, HASH_SEED));
	hash->update(handle->readAll());
	const QByteArray checksum = hash->digest(true);

	//Compare Hash
	if(qstricmp(checksum.constData(), expected_checksum) != 0)
	{
		qWarning("MediaInfo binary is corrupted!");
		qWarning("Expected checksum: %s", expected_checksum);
		qWarning("Computed checksum: %s\n", checksum.constData());
		return false;
	}

	qDebug("MediaInfo checksum: %s\n", checksum.constData());
	return true;
}

QPair<QString, const char*> CMainWindow::getMediaInfoArch(void)
{
	const MUtils::CPUFetaures::cpu_info_t cpu_features = MUtils::CPUFetaures::detect();
	for (size_t i = 0; MEDIAINFO_BIN[i].arch; ++i)
	{
		if (cpu_features.x64 || (!MEDIAINFO_BIN[i].require_x64))
		{
			if (HAVE_SSE2(cpu_features) || (!MEDIAINFO_BIN[i].require_sse2))
			{
				return qMakePair(QString::fromLatin1(MEDIAINFO_BIN[i].arch), MEDIAINFO_BIN[i].checksum);
			}
		}
	}
	MUTILS_THROW("This is not supposed to happen!");
}

void CMainWindow::selfTest(void)
{
	qWarning("[Self-Test]");
	for (size_t i = 0; MEDIAINFO_BIN[i].arch; ++i)
	{
		qWarning("%s: %s", MEDIAINFO_BIN[i].arch, MEDIAINFO_BIN[i].checksum);
		const QResource mediaInfoRes(QString(":/res/bin/MediaInfo.%1.exe").arg(QString::fromLatin1(MEDIAINFO_BIN[i].arch)));
		if (mediaInfoRes.isValid())
		{
			QScopedPointer<MUtils::Hash::Hash> hash(MUtils::Hash::create(MUtils::Hash::HASH_BLAKE2_512, HASH_SEED));
			hash->update(mediaInfoRes.data(), mediaInfoRes.size());
			const QByteArray checksum = hash->digest(true);
			if (qstricmp(MEDIAINFO_BIN[i].checksum, checksum.constData()) != 0)
			{
				qWarning("\nComputed checksum: %s\n", checksum.constData());
				qFatal("MediaInfo checksum mismatch detected!");
			}
		}
		else
		{
			qFatal("MediaInfo resource could not be found!");
		}
	}
	qWarning("Success.\n");
}

QString CMainWindow::getMediaInfoPath(void)
{
	//Detect MediaInfo arch
	const QPair<QString, const char*> arch = getMediaInfoArch();

	//Setup resource
	const QResource mediaInfoRes(QString(":/res/bin/MediaInfo.%1.exe").arg(arch.first));
	if((!mediaInfoRes.isValid()) || (!mediaInfoRes.data()))
	{
		qFatal("MediaInfo resource could not be initialized!");
		return QString();
	}
	
	//Validate file content, if already extracted
	if(!m_mediaInfoHandle.isNull())
	{
		if(VALIDATE_MEDIAINFO(m_mediaInfoHandle.data(), arch.second))
		{
			return m_mediaInfoHandle->fileName();
		}
		m_mediaInfoHandle->remove();
	}

	//Extract MediaInfo binary now!
	qDebug("MediaInfo binary not existing yet, going to extract now...\n");
	const QString filePath = MUtils::make_unique_file(m_tempFolder, "MediaInfo", arch.first + QLatin1String(".exe"));
	if (!filePath.isEmpty())
	{
		m_mediaInfoHandle.reset(new QFile(filePath));
		if (m_mediaInfoHandle->open(QIODevice::ReadWrite | QIODevice::Truncate))
		{
			if (m_mediaInfoHandle->write(reinterpret_cast<const char*>(mediaInfoRes.data()), mediaInfoRes.size()) == mediaInfoRes.size())
			{
				qDebug("MediaInfo path is:\n%s\n", m_mediaInfoHandle->fileName().toUtf8().constData());
				m_mediaInfoHandle->close();
				if (!m_mediaInfoHandle->open(QIODevice::ReadOnly))
				{
					qWarning("Failed to open MediaInfo binary for reading!\n");
					m_mediaInfoHandle->remove();
				}
			}
			else
			{
				qWarning("Failed to write data to MediaInfo binary file!\n");
				m_mediaInfoHandle->remove();
			}
		}
		else
		{
			qWarning("Failed to open MediaInfo binary for writing!\n");
		}
	}
	else
	{
		qWarning("Failed to gemerate MediaInfo outout path!\n");
	}

	//Validate file content, after it has been extracted
	if(!m_mediaInfoHandle.isNull())
	{
		if(VALIDATE_MEDIAINFO(m_mediaInfoHandle.data(), arch.second))
		{
			return m_mediaInfoHandle->fileName();
		}
		m_mediaInfoHandle->remove();
	}

	return QString();
}

QStringList CMainWindow::escapeHtmlLines(const QStringList &strings)
{
	QStringList output;
	for (QStringList::const_iterator iter = strings.constBegin(); iter != strings.constEnd(); ++iter)
	{
		output << Qt::escape(*iter);
	}
	return output;
}

QString CMainWindow::reformatXml(const QString &input)
{
	QString output;
	QXmlStreamReader reader(input);
	QXmlStreamWriter writer(&output);
	writer.setAutoFormatting(true);
	while (!reader.atEnd())
	{
		reader.readNext();
		if (!reader.isWhitespace())
		{
			writer.writeCurrentToken(reader);
		}
	}
	return output;
}
