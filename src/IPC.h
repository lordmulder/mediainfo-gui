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

#include <QThread>

class QSharedMemory;
class QSystemSemaphore;
class IPCSendThread;
class IPCReceiveThread;

class IPC : public QObject
{
	Q_OBJECT
	friend class IPCReceiveThread;
	friend class IPCSendThread;

public:
	IPC(void);
	~IPC(void);

	int initialize(void);
	bool sendAsync(const QString &str, const int timeout = 5000);

public slots:
	void startListening(void);
	void stopListening(void);

signals:
	void receivedStr(const QString &str);

protected:
	bool popStr(QString &str);
	bool pushStr(const QString &str);

	int m_initialized;

	QSharedMemory *m_sharedMemory;
	QSystemSemaphore *m_semaphoreRd;
	QSystemSemaphore *m_semaphoreWr;
	IPCReceiveThread *m_recvThread;
};

///////////////////////////////////////////////////////////////////////////////

class IPCSendThread : public QThread
{
	Q_OBJECT
	friend class IPC;

protected:
	IPCSendThread(IPC *ipc, const QString &str);
	inline bool result(void) { return m_result; }

	virtual void run(void);

private:
	volatile bool m_result;
	IPC *const m_ipc;
	const QString m_str;
};

class IPCReceiveThread : public QThread
{
	Q_OBJECT
	friend class IPC;

protected:
	IPCReceiveThread(IPC *ipc);
	inline void stop(void) { m_stopped = true; }

	virtual void run(void);

signals:
	void receivedStr(const QString &str);

private:
	void receiveLoop(void);
	volatile bool m_stopped;
	IPC *const m_ipc;
};
