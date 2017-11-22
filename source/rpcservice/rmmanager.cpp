#include "rmmanager.h"

#include "rpcerror.h"
#include "rmsession.h"
#include "rpcdistributor.h"
#include "rpcactionbase.h"
#include "rpcactlink.h"
#include "msmiddlewaredata.hpp"

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>
#include <QMetaType>


namespace MSRPC
{
	typedef QHash<QLatin1String, RpcActionBase*> HsAction;

	class RmComMgr::Impl
	{
	public:
		Impl()
			: bAutoDestroy(true)
		{}

		QThread thd;
		QPointer<QTcpServer> ptRmServer;

		QMap<unsigned int, RmSession*> mapSession;
		ReceiveDataDelegate dgReceiveData;

		HsAction hsAction;

		bool bAutoDestroy;
	};

	RmComMgr::RmComMgr()
		: QObject(0)
		, m_pImpl(new Impl)
	{
		this->moveToThread(&m_pImpl->thd);
		m_pImpl->thd.start();
		qRegisterMetaType< MsMiddleWareBase* >("MsMiddleWareBase*");
		qRegisterMetaType< QHostAddress >("QHostAddress");

		InitActions();
	}

	RmComMgr::~RmComMgr()
	{
		if (m_pImpl->thd.isRunning())
		{
			m_pImpl->thd.quit();
			m_pImpl->thd.wait(6);
		}
	}

	void RmComMgr::Listen(const QString& strHostAddr, quint16 uPort)
	{
		qDebug() << QThread::currentThreadId();
		QMetaObject::invokeMethod(this, "slot_Listen", 
			Q_ARG(QString, strHostAddr), Q_ARG(quint16, uPort));
	}

	void RmComMgr::slot_Listen(const QString& strHostAddr, quint16 uPort)
	{
		qDebug() << QThread::currentThreadId();

		do
		{
			if (!m_pImpl->ptRmServer)
			{
				m_pImpl->ptRmServer = new QTcpServer(this);
			}

			if (m_pImpl->ptRmServer->isListening())
			{
				emit signal_ListenResult(EC_BEING_LISTENED);
				break;
			}

			if (!m_pImpl->ptRmServer->listen(
				QHostAddress(strHostAddr), uPort))
			{
				emit signal_ListenResult(EC_LISTEN_FAILURE);
				break;
			}

			QObject::connect(m_pImpl->ptRmServer.data(), SIGNAL(newConnection()),
				this, SLOT(slot_NewConnection()));

			emit signal_ListenResult(EC_SUCCESS);

		} while (0);
	}

	void RmComMgr::slot_Disconnect(unsigned int uSID)
	{
		QMap<unsigned int, RmSession*>::const_iterator ciFind =
			m_pImpl->mapSession.constFind(uSID);

		if (ciFind != m_pImpl->mapSession.constEnd())
		{
			(*ciFind)->CloseSession();
		}
	}

	void RmComMgr::slot_NewConnection()
	{
		QTcpSocket* pSocket = m_pImpl->ptRmServer->nextPendingConnection();

		///组装会话
		RmSession* pSession = CreateSession(pSocket);
		emit signal_SessionStart(pSession->GetSID());
	}

	QTcpServer* RmComMgr::GetServer() const
	{
		return m_pImpl->ptRmServer.data();
	}

	RmSession* RmComMgr::GetRmSession(unsigned int uSID) const
	{
		return m_pImpl->mapSession.value(uSID);
	}

	QThread* RmComMgr::GetThread() const
	{
		return &m_pImpl->thd;
	}

	void RmComMgr::SendData(unsigned int uSID, const QByteArray& baData, quint8 eType)
	{
		if (&m_pImpl->thd == QThread::currentThread())
		{
			QMap<unsigned int, RmSession*>::const_iterator ciFind =
				m_pImpl->mapSession.constFind(uSID);

			if (ciFind != m_pImpl->mapSession.constEnd())
			{
				(*ciFind)->SendData(baData, eType);
			}
		}
		else
		{
			QMetaObject::invokeMethod(this, "SendData",
				Q_ARG(unsigned int, uSID),
				Q_ARG(QByteArray, baData),
				Q_ARG(quint8, eType));
		}
	}

	void RmComMgr::Connect(const QString& strHostName, quint16 uPort)
	{
		QMetaObject::invokeMethod(this, "slot_Connect",
			Q_ARG(QString, strHostName), Q_ARG(quint16, uPort));
	}

	void RmComMgr::Disconnect(unsigned int uSID)
	{
		QMetaObject::invokeMethod(this, "slot_Disconnect",
			Q_ARG(unsigned int, uSID));
	}

	void RmComMgr::slot_Connect(const QString& strHostName, quint16 uPort)
	{
		do
		{
			QTcpSocket* pSocket = new QTcpSocket(this);
			pSocket->connectToHost(QHostAddress(strHostName), uPort);
			if (!pSocket->waitForConnected(5000))
			{
				emit signal_ConnectResult(EC_CONNECTREMOTE_FAILURE);
				delete pSocket;
				break;
			}

			///组装会话
			RmSession* pSession = CreateSession(pSocket);
			emit signal_SessionStart(pSession->GetSID());

			emit signal_ConnectResult(EC_SUCCESS);

		} while (0);
	}

	RmSession* RmComMgr::CreateSession(QTcpSocket* spSocket)
	{
		unsigned int uSID = reinterpret_cast<unsigned int>(spSocket);
		RmSession* pRmSession = new RmSession(uSID, spSocket);
		pRmSession->SetReceiveDataDelegate(m_pImpl->dgReceiveData);
		m_pImpl->mapSession[uSID] = pRmSession;
		QObject::connect(pRmSession, SIGNAL(destroyed(QObject*)),
			this, SLOT(slot_SessionDestroyed(QObject*)));
		QObject::connect(pRmSession, SIGNAL(signal_SocketError()),
			this, SLOT(slot_SessionError()));
		
		return pRmSession;
	}

	void RmComMgr::SetReceiveDataDelegate(const ReceiveDataDelegate& dgReceiveData)
	{
		m_pImpl->dgReceiveData = dgReceiveData;
	}

	void RmComMgr::CloseAll()
	{
		foreach(RmSession* pSession, m_pImpl->mapSession)
		{
			pSession->CloseSession();
		}
	}

	void RmComMgr::slot_SessionDestroyed(QObject *obj)
	{
		unsigned int uSID = m_pImpl->mapSession.key(static_cast<RmSession*>(obj), 0);
		if (uSID != 0)
		{
			m_pImpl->mapSession.remove(uSID);
			emit signal_SessionEnd(uSID);
		}
	}

	void RmComMgr::slot_SessionError()
	{
		if (RmSession* pSession = qobject_cast<RmSession*>(sender()))
		{
			emit signal_SessionError(pSession->GetSID());

			if (m_pImpl->bAutoDestroy)
			{
				pSession->CloseSession();
			}
		}
	}

	void RmComMgr::RegRpcActionBase(RpcActionBase* pRmAction)
	{
		m_pImpl->hsAction[QLatin1String(pRmAction->GetActionName())] = pRmAction;
	}

	MSRPC::RpcActionBase* RmComMgr::GetRpcActionBase(const char* RmActionName) const
	{
		return m_pImpl->hsAction.value(QLatin1String(RmActionName), 0);
	}

	void RmComMgr::InitActions()
	{
		RpcActLink* p = RegRpcAction<RpcActLink>();
		p->SetManager(this, 
			fastdelegate::MakeDelegate(this, &RmComMgr::IsIdle));
	}

	void RmComMgr::SetAutoDestroyed(bool bAutoDestroy)
	{
		m_pImpl->bAutoDestroy = bAutoDestroy;
	}

	bool RmComMgr::GetAutoDestroyed() const
	{
		return m_pImpl->bAutoDestroy;
	}

	const QMap<unsigned int, RmSession*>& RmComMgr::GetRmSessions() const
	{
		return m_pImpl->mapSession;
	}

	bool RmComMgr::IsIdle() const
	{
		return m_pImpl->mapSession.isEmpty();
	}

}