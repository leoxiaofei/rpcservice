#include "rpcreportbase.h"
#include "rapidjson/msjson/msjsonserialize.hpp"
#include "msmiddlewaredata.hpp"


namespace MSRPC
{
	void RpcReportBase::ConnectReceiver(QObject* objReceiver, const char* szFunctor)
	{
		MapReceiver::iterator iFind = m_mapReceiver.find(objReceiver);

		if (iFind == m_mapReceiver.end())
		{
			QObject::connect(objReceiver, SIGNAL(destroyed(QObject*)), this, SLOT(slot_ReceiverDestroy(QObject*)));

			iFind = m_mapReceiver.insert(objReceiver, QVector<const char *>());
		}

		iFind->append(szFunctor);
	}

	void RpcReportBase::DisconnectReceiver(QObject* objReceiver, const char* szFunctor)
	{
		MapReceiver::iterator iFind = m_mapReceiver.find(objReceiver);

		if (iFind != m_mapReceiver.end())
		{
			iFind->remove(iFind->indexOf(szFunctor));
			if (iFind->isEmpty())
			{
				QObject::disconnect(objReceiver, SIGNAL(destroyed(QObject*)), this, SLOT(slot_ReceiverDestroy(QObject*)));
				iFind = m_mapReceiver.erase(iFind);
			}

		}
	}

	void RpcReportBase::SetReportDelegate(const SendReportDelegate& dgSendReport)
	{
		m_dgSendReport = dgSendReport;
	}

	void RpcReportBase::ActSend(unsigned int uSID, unsigned int uVersion, MsMiddleWareBase* pBase)
	{
		pBase->SetVersion(uVersion);
		m_dgSendReport(uSID, this, pBase);
		delete pBase;
	}

	void RpcReportBase::ActRecv(unsigned int uSID, const qint64& nTime, MsMiddleWareBase* pBase)
	{
		emit signal_ActRecv(uSID, nTime, QSharedPointer<MsMiddleWareBase>(pBase));
	}

	void RpcReportBase::slot_ReceiverDestroy(QObject* objReceiver)
	{
		m_mapReceiver.remove(objReceiver);
	}

}

