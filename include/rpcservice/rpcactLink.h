#ifndef ACTLINK_H__
#define ACTLINK_H__

#include "rpcactionbase.h"

namespace MSRPC
{
	///unsigned int 连接ID，bool 连接/断开。
	typedef fastdelegate::FastDelegate2<unsigned int, bool> LinkStateDelegate;

	///通知没有连接了。
	typedef fastdelegate::FastDelegate0<> NoLinkDelegate;

	///空闲Idle
	typedef fastdelegate::FastDelegate0<bool> IdleDelegate;

class RPCSERVICE_EXPORT RpcActLink : public RpcActionBase
{
	Q_OBJECT
	ActionName(RpcActLink);
public:
	bool Listen(const QString& strHostAddr, quint16 uPort);
	bool Connect(const QString& strHostName, quint16 uPort);
	bool Disconnect(unsigned int uSID);

	void SetStateDelegate(const LinkStateDelegate& lsd);
	void SetNoLinkDelegate(const NoLinkDelegate& led);

public:
	void SetManager(QObject* pManager, const IdleDelegate& dgIdle);

protected slots:
	void slot_SessionStart(unsigned int uSID);
	void slot_SessionEnd(unsigned int uSID);

private:
	LinkStateDelegate m_dgLinkState;
	NoLinkDelegate m_dgNoLink;
	IdleDelegate m_dgIdle;
	QObject* m_pManager;
};

}

#endif // ACTLINK_H__
