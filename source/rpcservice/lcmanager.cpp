#include "lcmanager.h"

#include "rpcerror.h"
#include "lcsession.h"
#include "rpcdistributor.h"
#include "rpcactionbase.h"
#include "rpcactlink.h"
#include "msmiddlewaredata.hpp"

#include <QThread>
#include <QLocalServer>
#include <QLocalSocket>
#include <QPointer>
#include <QMetaType>


namespace MSRPC
{
	typedef QHash<QLatin1String, RpcActionBase*> HsAction;

	inline QString GetLocalName(const QString& strHostAddr, const quint16& uPort)
	{
		return strHostAddr + QString::number(uPort);
	}

	class LcComMgr::Impl
	{
	public:
		Impl()
			: bAutoDestroy(true)
		{}

		QThread thd;
		QPointer<QLocalServer> ptServer;

		QMap<unsigned int, LcSession*> mapSession;
		ReceiveDataDelegate dgReceiveData;

		HsAction hsAction;

		bool bAutoDestroy;
	};

	LcComMgr::LcComMgr()
		: QObject(0)
		, m_pImpl(new Impl)
	{
		this->moveToThread(&m_pImpl->thd);
		m_pImpl->thd.start();
		qRegisterMetaType< MsMiddleWareBase* >("MsMiddleWareBase*");

		InitActions();
	}

	LcComMgr::~LcComMgr()
	{
		if (m_pImpl->thd.isRunning())
		{
			m_pImpl->thd.quit();
			m_pImpl->thd.wait(6);
		}
	}

	void LcComMgr::Listen(const QString& strHostAddr, quint16 uPort)
	{
		qDebug() << QThread::currentThreadId();
		QMetaObject::invokeMethod(this, "slot_Listen", 
			Q_ARG(QString, strHostAddr), Q_ARG(quint16, uPort));
	}

	void LcComMgr::slot_Listen(const QString& strHostAddr, quint16 uPort)
	{
		qDebug() << QThread::currentThreadId();

		do
		{
			if (!m_pImpl->ptServer)
			{
				m_pImpl->ptServer = new QLocalServer(this);
			}

			if (m_pImpl->ptServer->isListening())
			{
				emit signal_ListenResult(EC_BEING_LISTENED);
				break;
			}

			if (!m_pImpl->ptServer->listen(
				GetLocalName(strHostAddr, uPort)))
			{
				emit signal_ListenResult(EC_LISTEN_FAILURE);
				break;
			}

			QObject::connect(m_pImpl->ptServer.data(), SIGNAL(newConnection()),
				this, SLOT(slot_NewConnection()));

			emit signal_ListenResult(EC_SUCCESS);

		} while (0);

	}

	void LcComMgr::slot_Disconnect(unsigned int uSID)
	{
		QMap<unsigned int, LcSession*>::const_iterator ciFind =
			m_pImpl->mapSession.constFind(uSID);

		if (ciFind != m_pImpl->mapSession.constEnd())
		{
			(*ciFind)->CloseSession();
		}
	}

	void LcComMgr::slot_NewConnection()
	{
		QLocalSocket* pSocket = m_pImpl->ptServer->nextPendingConnection();

		///组装会话
		LcSession* pSession = CreateSession(pSocket);
		emit signal_SessionStart(pSession->GetSID());
	}

	QLocalServer* LcComMgr::GetServer() const
	{
		return m_pImpl->ptServer.data();
	}

	LcSession* LcComMgr::GetLcSession(unsigned int uSID) const
	{
		return m_pImpl->mapSession.value(uSID);
	}

	QThread* LcComMgr::GetThread() const
	{
		return &m_pImpl->thd;
	}

	void LcComMgr::SendData(unsigned int uSID, const QByteArray& baData, quint8 eType)
	{
		if (&m_pImpl->thd == QThread::currentThread())
		{
			QMap<unsigned int, LcSession*>::const_iterator ciFind =
				m_pImpl->mapSession.constFind(uSID);

			if (ciFind != m_pImpl->mapSession.constEnd())
			{
				//qDebug()<<baData;
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

	void LcComMgr::Connect(const QString& strHostName, quint16 uPort)
	{
		QMetaObject::invokeMethod(this, "slot_Connect",
			Q_ARG(QString, strHostName), Q_ARG(quint16, uPort));
	}

	void LcComMgr::Disconnect(unsigned int uSID)
	{
		QMetaObject::invokeMethod(this, "slot_Disconnect",
			Q_ARG(unsigned int, uSID));
	}

	void LcComMgr::slot_Connect(const QString& strHostName, quint16 uPort)
	{
		do
		{
			QLocalSocket* pSocket = new QLocalSocket(this);
			pSocket->connectToServer(GetLocalName(strHostName, uPort));
			if (!pSocket->waitForConnected(5000))
			{
				emit signal_ConnectResult(EC_CONNECTREMOTE_FAILURE);
				delete pSocket;
				break;
			}

			///组装会话
			LcSession* pSession = CreateSession(pSocket);
			emit signal_SessionStart(pSession->GetSID());

			emit signal_ConnectResult(EC_SUCCESS);

		} while (0);
	}

	LcSession* LcComMgr::CreateSession(QLocalSocket* spSocket)
	{
		unsigned int uSID = reinterpret_cast<unsigned int>(spSocket);
		LcSession* pLcSession = new LcSession(uSID, spSocket);
		pLcSession->SetReceiveDataDelegate(m_pImpl->dgReceiveData);
		m_pImpl->mapSession[uSID] = pLcSession;
		QObject::connect(pLcSession, SIGNAL(destroyed(QObject*)),
			this, SLOT(slot_SessionDestroyed(QObject*)));
		QObject::connect(pLcSession, SIGNAL(signal_SocketError()),
			this, SLOT(slot_SessionError()));
		
		return pLcSession;
	}

	void LcComMgr::CloseAll()
	{
		foreach(LcSession* pSession, m_pImpl->mapSession)
		{
			pSession->CloseSession();
		}
	}

	void LcComMgr::slot_SessionDestroyed(QObject *obj)
	{
		unsigned int uSID = m_pImpl->mapSession.key(static_cast<LcSession*>(obj), 0);
		if (uSID != 0)
		{
			m_pImpl->mapSession.remove(uSID);
			emit signal_SessionEnd(uSID);
		}
	}

	void LcComMgr::slot_SessionError()
	{
		if (LcSession* pSession = qobject_cast<LcSession*>(sender()))
		{
			emit signal_SessionError(pSession->GetSID());

			if (m_pImpl->bAutoDestroy)
			{
				pSession->CloseSession();
			}
		}
	}

	void LcComMgr::RegRpcActionBase(RpcActionBase* pRmAction)
	{
		m_pImpl->hsAction[QLatin1String(pRmAction->GetActionName())] = pRmAction;
	}

	MSRPC::RpcActionBase* LcComMgr::GetRpcActionBase(const char* RmActionName) const
	{
		return m_pImpl->hsAction.value(QLatin1String(RmActionName), 0);
	}

	void LcComMgr::InitActions()
	{
		RpcActLink* p = RegRpcAction<RpcActLink>();
		p->SetManager(this,	fastdelegate::MakeDelegate(this, &LcComMgr::IsIdle));
	}

	void LcComMgr::SetAutoDestroyed(bool bAutoDestroy)
	{
		m_pImpl->bAutoDestroy = bAutoDestroy;
	}

	bool LcComMgr::GetAutoDestroyed() const
	{
		return m_pImpl->bAutoDestroy;
	}

	const QMap<unsigned int, LcSession*>& LcComMgr::GetLcSessions() const
	{
		return m_pImpl->mapSession;
	}

	bool LcComMgr::IsIdle() const
	{
		return m_pImpl->mapSession.isEmpty();
	}

}