#include "rpcactlink.h"
#include "mstools\rtcodeloop.h"
#include "rmmanager.h"

namespace MSRPC
{
	bool RpcActLink::Listen(const QString& strHostAddr, quint16 uPort)
	{
		RtCodeLoop rcLoop;

		QObject::connect(m_pManager, SIGNAL(signal_ListenResult(int)), &rcLoop, SLOT(slot_Finish(int)));
		
		QMetaObject::invokeMethod(m_pManager, "slot_Listen",
			Q_ARG(QString, strHostAddr), Q_ARG(quint16, uPort));
		
		return rcLoop.exec() == 0;
	}

	bool RpcActLink::Connect(const QString& strHostName, quint16 uPort)
	{
		RtCodeLoop rcLoop;

		QObject::connect(m_pManager, SIGNAL(signal_ConnectResult(int)), &rcLoop, SLOT(slot_Finish(int)));

		QMetaObject::invokeMethod(m_pManager, "slot_Connect",
			Q_ARG(QString, strHostName), Q_ARG(quint16, uPort));

		return rcLoop.exec() == 0;
	}

	bool RpcActLink::Disconnect(unsigned int uSID)
	{
		QMetaObject::invokeMethod(m_pManager, "slot_Disconnect",
			Q_ARG(unsigned int, uSID));

		return true;
	}

	void RpcActLink::SetStateDelegate(const LinkStateDelegate& lsd)
	{
		m_dgLinkState = lsd;
	}

	void RpcActLink::SetNoLinkDelegate(const NoLinkDelegate& led)
	{
		m_dgNoLink = led;
	}

	void RpcActLink::SetManager(QObject* pManager, const IdleDelegate& dgIdle)
	{
		m_pManager = pManager;
		m_dgIdle = dgIdle;

		QObject::connect(m_pManager, SIGNAL(signal_SessionStart(unsigned int)),
			this, SLOT(slot_SessionStart(unsigned int)));

		QObject::connect(m_pManager, SIGNAL(signal_SessionEnd(unsigned int)),
			this, SLOT(slot_SessionEnd(unsigned int)));
	}

	void RpcActLink::slot_SessionStart(unsigned int uSID)
	{
		if (m_dgLinkState)
		{
			m_dgLinkState(uSID, true);
		}
	}

	void RpcActLink::slot_SessionEnd(unsigned int uSID)
	{
		if (m_dgLinkState)
		{
			m_dgLinkState(uSID, false);
		}

		if (m_dgNoLink && m_dgIdle())
		{
			m_dgNoLink();
		}
	}

}
