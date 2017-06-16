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

	class LcManager::Impl
	{
	public:
		Impl()
			: bAutoDestroy(true)
		{}

		QThread thd;
		QPointer<QLocalServer> ptServer;

		QMap<unsigned int, LcSession*> mapSession;
		QMap<quint8, RpcDistributor*> mapDistributor;

		HsAction hsAction;

		bool bAutoDestroy;
	};

	LcManager::LcManager()
		: QObject(0)
		, m_pImpl(new Impl)
	{
		this->moveToThread(&m_pImpl->thd);
		m_pImpl->thd.start();
		qRegisterMetaType< MsMiddleWareBase* >("MsMiddleWareBase*");

		InitActions();
	}

	LcManager::~LcManager()
	{
		if (m_pImpl->thd.isRunning())
		{
			m_pImpl->thd.quit();
			m_pImpl->thd.wait(6);
		}
	}

	void LcManager::Listen(const QString& strHostAddr, quint16 uPort)
	{
		qDebug() << QThread::currentThreadId();
		QMetaObject::invokeMethod(this, "slot_Listen", 
			Q_ARG(QString, strHostAddr), Q_ARG(quint16, uPort));
	}

	void LcManager::slot_Listen(const QString& strHostAddr, quint16 uPort)
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

	void LcManager::slot_Disconnect(unsigned int uSID)
	{
		QMap<unsigned int, LcSession*>::const_iterator ciFind =
			m_pImpl->mapSession.constFind(uSID);

		if (ciFind != m_pImpl->mapSession.constEnd())
		{
			(*ciFind)->CloseSession();
		}
	}

	void LcManager::slot_NewConnection()
	{
		QLocalSocket* pSocket = m_pImpl->ptServer->nextPendingConnection();

		///组装会话
		LcSession* pSession = CreateSession(pSocket);
		emit signal_SessionStart(pSession->GetSID());
	}

	QLocalServer* LcManager::GetServer() const
	{
		return m_pImpl->ptServer.data();
	}

	LcSession* LcManager::GetLcSession(unsigned int uSID) const
	{
		return m_pImpl->mapSession.value(uSID);
	}

	QThread* LcManager::GetThread() const
	{
		return &m_pImpl->thd;
	}

	void LcManager::SendData(unsigned int uSID, const QByteArray& baData, quint8 eType)
	{
		if (&m_pImpl->thd == QThread::currentThread())
		{
			QMap<unsigned int, LcSession*>::const_iterator ciFind =
				m_pImpl->mapSession.constFind(uSID);

			if (ciFind != m_pImpl->mapSession.constEnd())
			{
				qDebug()<<baData;
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

	void LcManager::Connect(const QString& strHostName, quint16 uPort)
	{
		QMetaObject::invokeMethod(this, "slot_Connect",
			Q_ARG(QString, strHostName), Q_ARG(quint16, uPort));
	}

	void LcManager::Disconnect(unsigned int uSID)
	{
		QMetaObject::invokeMethod(this, "slot_Disconnect",
			Q_ARG(unsigned int, uSID));
	}

	void LcManager::slot_Connect(const QString& strHostName, quint16 uPort)
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

	LcSession* LcManager::CreateSession(QLocalSocket* spSocket)
	{
		unsigned int uSID = reinterpret_cast<unsigned int>(spSocket);
		LcSession* pLcSession = new LcSession(uSID, spSocket);
		pLcSession->SetReceiveDataDelegate(
			fastdelegate::MakeDelegate(this, &LcManager::ReceiveData));
		m_pImpl->mapSession[uSID] = pLcSession;
		QObject::connect(pLcSession, SIGNAL(destroyed(QObject*)),
			this, SLOT(slot_SessionDestroyed(QObject*)));
		QObject::connect(pLcSession, SIGNAL(signal_SocketError()),
			this, SLOT(slot_SessionError()));
		
		return pLcSession;
	}

	void LcManager::CloseAll()
	{
		foreach(LcSession* pSession, m_pImpl->mapSession)
		{
			pSession->CloseSession();
		}
	}

	void LcManager::slot_SessionDestroyed(QObject *obj)
	{
		unsigned int uSID = m_pImpl->mapSession.key(static_cast<LcSession*>(obj), 0);
		if (uSID != 0)
		{
			m_pImpl->mapSession.remove(uSID);
			emit signal_SessionEnd(uSID);
		}
	}

	void LcManager::slot_SessionError()
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

	void LcManager::ReceiveData(const LcSession* rSession, QByteArray& baData, quint8 eType)
	{
		unsigned int uSID = rSession->GetSID();
		QMap<quint8, RpcDistributor*>::const_iterator ciFind = m_pImpl->mapDistributor.constFind(eType);
		if (ciFind != m_pImpl->mapDistributor.constEnd())
		{
			qDebug()<<baData;
			(*ciFind)->ReceiveData(uSID, baData);
		}
		else
		{
			emit signal_ReceiveData(uSID, baData, eType);
		}
	}

	void LcManager::RegDistributor(RpcDistributor* pRmDistributor)
	{
		pRmDistributor->SetSendDataDelegate(fastdelegate::MakeDelegate(this, &MSRPC::LcManager::SendData));
		m_pImpl->mapDistributor[pRmDistributor->GetType()] = pRmDistributor;
	}

	MSRPC::RpcDistributor* LcManager::GetDistributor(int nRmDisType) const
	{
		return m_pImpl->mapDistributor.value(nRmDisType, 0);
	}

	void LcManager::RegRpcActionBase(RpcActionBase* pRmAction)
	{
		m_pImpl->hsAction[QLatin1String(pRmAction->GetActionName())] = pRmAction;
	}

	MSRPC::RpcActionBase* LcManager::GetRpcActionBase(const char* RmActionName) const
	{
		return m_pImpl->hsAction.value(QLatin1String(RmActionName), 0);
	}

	void LcManager::InitActions()
	{
		RpcActLink* p = RegRpcAction<RpcActLink>();
		p->SetManager(this,	fastdelegate::MakeDelegate(this, &LcManager::IsIdle));
	}

	void LcManager::SetAutoDestroyed(bool bAutoDestroy)
	{
		m_pImpl->bAutoDestroy = bAutoDestroy;
	}

	bool LcManager::GetAutoDestroyed() const
	{
		return m_pImpl->bAutoDestroy;
	}

	const QMap<unsigned int, LcSession*>& LcManager::GetLcSessions() const
	{
		return m_pImpl->mapSession;
	}

	bool LcManager::IsIdle() const
	{
		return m_pImpl->mapSession.isEmpty();
	}

}