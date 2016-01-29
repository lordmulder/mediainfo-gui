///////////////////////////////////////////////////////////////////////////////
// MediaInfoXP
// Copyright (C) 2004-2016 LoRd_MuldeR <MuldeR2@GMX.de>
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

namespace MUtils
{
	class IPCChannel;
}

///////////////////////////////////////////////////////////////////////////////

class IPCSendThread : public QThread
{
	Q_OBJECT

public:
	IPCSendThread(MUtils::IPCChannel *const ipc, const quint32 &command, const QString &message);
	inline bool result(void) { return m_result; }

	virtual void run(void);

private:
	volatile bool m_result;
	MUtils::IPCChannel *const m_ipc;
	const quint32 m_command;
	const QString m_message;
};

class IPCReceiveThread : public QThread
{
	Q_OBJECT

public:
	IPCReceiveThread(MUtils::IPCChannel *const ipc);
	void stop(void);

protected:
	virtual void run(void);

signals:
	void received(const quint32 &command, const QString &message);

private:
	void receiveLoop(void);
	volatile bool m_stopped;
	MUtils::IPCChannel *const m_ipc;
};

class IPC
{
public:
	enum
	{
		COMMAND_NONE = 0,
		COMMAND_PING = 1,
		COMMAND_OPEN = 2
	}
	ipc_command_t;

	static bool sendAsync(MUtils::IPCChannel *const ipc, const quint32 &command, const QString &message, const quint32 &timeout = 5000);

private:
	IPC(void) { throw 666; }
};
