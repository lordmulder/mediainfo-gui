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

#include "IPC.h"

#include "Utils.h"

#include <QSharedMemory>
#include <QSystemSemaphore>

static const size_t MAX_STR_LEN = 1024;
static const size_t MAX_ENTRIES = 16;

static const char *s_key_smemory = "{35DE4BDD-F88C-41E1-8080-2EDC503757F2}";
static const char *s_key_sema_wr = "{C638D990-7553-4D4D-95E8-9B040BA6AE87}";
static const char *s_key_sema_rd = "{0C30B0B2-0B03-49D7-8DB3-A7D0DDDEA2B0}";

typedef struct
{
	wchar_t data[MAX_ENTRIES][MAX_STR_LEN];
	size_t posRd;
	size_t posWr;
	size_t counter;
}
mixp_ipc_t;

///////////////////////////////////////////////////////////////////////////////
// Send Thread
///////////////////////////////////////////////////////////////////////////////

IPCSendThread::IPCSendThread(IPC *ipc, const QString &str)
:
	m_ipc(ipc), m_str(str)
{
	m_result = false;
}

void IPCSendThread::run(void)
{
	try
	{
		m_result = m_ipc->pushStr(m_str);
	}
	catch(...)
	{
		m_result = false;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Receive Thread
///////////////////////////////////////////////////////////////////////////////

IPCReceiveThread::IPCReceiveThread(IPC *ipc)
:
	m_ipc(ipc)
{
	m_stopped = false;
}
	
void IPCReceiveThread::run(void)
{
	try
	{
		receiveLoop();
	}
	catch(...)
	{
		qWarning("Exception in IPC receive thread!");
	}
}

void IPCReceiveThread::receiveLoop(void)
{
	while(!m_stopped)
	{
		QString temp;
		if(m_ipc->popStr(temp))
		{
			if(!temp.isEmpty())
			{
				emit receivedStr(temp);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// IPC Class
///////////////////////////////////////////////////////////////////////////////

IPC::IPC(void)
{
	m_initialized  = -1;
	m_sharedMemory = NULL;
	m_semaphoreWr  = NULL;
	m_semaphoreRd  = NULL;
	m_recvThread   = NULL;
}

IPC::~IPC(void)
{
	if(m_recvThread && m_recvThread->isRunning())
	{
		qWarning("Receive thread still running -> terminating!");
		m_recvThread->terminate();
		m_recvThread->wait();
	}
	
	MIXP_DELETE_OBJ(m_recvThread);
	MIXP_DELETE_OBJ(m_sharedMemory);
	MIXP_DELETE_OBJ(m_semaphoreWr);
	MIXP_DELETE_OBJ(m_semaphoreRd);
}

int IPC::initialize(void)
{
	if(m_initialized >= 0)
	{
		return m_initialized;
	}

	m_semaphoreWr = new QSystemSemaphore(s_key_sema_wr, MAX_ENTRIES);
	m_semaphoreRd = new QSystemSemaphore(s_key_sema_rd, 0);

	if((m_semaphoreWr->error() != QSystemSemaphore::NoError) || (m_semaphoreRd->error() != QSystemSemaphore::NoError))
	{
		qWarning("IPC: Failed to created system semaphores!");
		return -1;
	}

	m_sharedMemory = new QSharedMemory(s_key_smemory, this);

	if(m_sharedMemory->create(sizeof(mixp_ipc_t)))
	{
		memset(m_sharedMemory->data(), 0, sizeof(mixp_ipc_t));
		m_initialized = 1;
		return 1;
	}

	if(m_sharedMemory->error() == QSharedMemory::AlreadyExists)
	{
		qDebug("Not the first instance -> attaching to existing shared memory");
		if(m_sharedMemory->attach())
		{
			m_initialized = 0;
			return 0;
		}
	}

	qWarning("IPC: Failed to attach to the shared memory!");
	return -1;
}

bool IPC::pushStr(const QString &str)
{
	if(m_initialized < 0)
	{
		qWarning("Error: IPC not initialized yet!");
		return false;
	}

	if(!m_semaphoreWr->acquire())
	{
		qWarning("IPC: Failed to acquire semaphore!");
		return false;
	}

	if(!m_sharedMemory->lock())
	{
		qWarning("IPC: Failed to lock shared memory!");
		return false;
	}

	bool success = true;

	try
	{
		mixp_ipc_t *memory = (mixp_ipc_t*) m_sharedMemory->data();
		if(memory->counter < MAX_ENTRIES)
		{
			wcsncpy_s(memory->data[memory->posWr], MAX_STR_LEN, (wchar_t*)str.utf16(), _TRUNCATE);
			memory->posWr = (memory->posWr + 1) % MAX_ENTRIES;
			memory->counter++;
		}
		else
		{
			qWarning("IPC: Shared memory is full -> cannot push string!");
			success = false;
		}
	}
	catch(...)
	{
		/*ignore any exception*/
	}

	m_sharedMemory->unlock();

	if(success)
	{
		m_semaphoreRd->release();
	}

	return success;
}

bool IPC::popStr(QString &str)
{
	if(m_initialized < 0)
	{
		qWarning("Error: IPC not initialized yet!");
		return false;
	}

	if(!m_semaphoreRd->acquire())
	{
		qWarning("IPC: Failed to acquire semaphore!");
		return false;
	}

	if(!m_sharedMemory->lock())
	{
		qWarning("IPC: Failed to lock shared memory!");
		return false;
	}

	bool success = true;

	try
	{
		mixp_ipc_t *memory = (mixp_ipc_t*) m_sharedMemory->data();
		if(memory->counter > 0)
		{
			memory->data[memory->posRd][MAX_STR_LEN-1] = L'\0';
			str = QString::fromUtf16((const ushort*)memory->data[memory->posRd]);
			memory->posRd = (memory->posRd + 1) % MAX_ENTRIES;
			memory->counter--;
		}
		else
		{
			qWarning("IPC: Shared memory is empty -> cannot pop string!");
			success = false;
		}
	}
	catch(...)
	{
		/*ignore any exception*/
	}

	m_sharedMemory->unlock();

	if(success)
	{
		m_semaphoreWr->release();
	}

	return success;
}

bool IPC::sendAsync(const QString &str, const int timeout)
{
	IPCSendThread sendThread(this, str);
	sendThread.start();

	if(!sendThread.wait(timeout))
	{
		qWarning("IPC send operation encountered timeout!");
		sendThread.terminate();
		sendThread.wait();
		return false;
	}

	return sendThread.result();
}

void IPC::startListening(void)
{
	if(!m_recvThread)
	{
		m_recvThread = new IPCReceiveThread(this);
		connect(m_recvThread, SIGNAL(receivedStr(QString)), this, SIGNAL(receivedStr(QString)), Qt::QueuedConnection);
	}

	if(!m_recvThread->isRunning())
	{
		m_recvThread->start();
	}
	else
	{
		qWarning("Receive thread was already running!");
	}

}

void IPC::stopListening(void)
{
	if(m_recvThread && m_recvThread->isRunning())
	{
		m_recvThread->stop();
		m_semaphoreRd->release();

		if(!m_recvThread->wait(5000))
		{
			qWarning("Receive thread seems deadlocked -> terminating!");
			m_recvThread->terminate();
			m_recvThread->wait();
		}
	}
	else
	{
		qWarning("Receive thread was not running!");
	}
}
