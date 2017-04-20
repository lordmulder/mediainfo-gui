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

#include "IPC.h"

//MUtils
#include "MUtils/Global.h"
#include "MUtils/IPCChannel.h"

//Qt
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

IPCSendThread::IPCSendThread(MUtils::IPCChannel *const ipc, const quint32 &command, const QString &message)
:
	m_ipc(ipc), m_command(command), m_message(message)
{
}

void IPCSendThread::run(void)
{
	try
	{
		m_result.fetchAndStoreOrdered(m_ipc->send(m_command, 0, QStringList() << m_message) ? 1 : 0);
	}
	catch(...)
	{
		qWarning("Exception in IPC receive thread!");
		m_result.fetchAndStoreOrdered(0);
	}
}


///////////////////////////////////////////////////////////////////////////////
// Receive Thread
///////////////////////////////////////////////////////////////////////////////

IPCReceiveThread::IPCReceiveThread(MUtils::IPCChannel *const ipc)
:
	m_ipc(ipc)
{
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
		quint32 command, flags;
		QStringList params;
		if(m_ipc->read(command, flags, params))
		{
			if((command != IPC::COMMAND_NONE) && (!params.isEmpty()))
			{
				emit received(command, params.first());
			}
		}
	}
}

void IPCReceiveThread::stop(void)
{
	if(m_stopped.testAndSetOrdered(0, 1))
	{
		IPC::sendAsync(m_ipc, IPC::COMMAND_NONE, "exit");
	}
}

///////////////////////////////////////////////////////////////////////////////
// IPC Class
///////////////////////////////////////////////////////////////////////////////

bool IPC::sendAsync(MUtils::IPCChannel *const ipc, const quint32 &command, const QString &message, const quint32 &timeout)
{
	IPCSendThread sendThread(ipc, command, message);
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
