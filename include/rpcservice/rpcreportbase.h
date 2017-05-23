#ifndef __RMREPORTBASE_H__
#define __RMREPORTBASE_H__

#include "rpcservice_global.h"
#include "mstools/FastDelegate.h"

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QVector>

namespace MSRPC
{
	class MsMiddleWareBase;
	class IArchiveBase;

	typedef fastdelegate::FastDelegate3<unsigned int, const class RpcReportBase*, MsMiddleWareBase*> SendReportDelegate;

	class RPCSERVICE_EXPORT RpcReportBase : public QObject
	{
		Q_OBJECT
	public:
		virtual const char* GetName() const = 0;

		void ConnectReceiver(QObject* objReceiver, const char* szFunctor);
		void DisconnectReceiver(QObject* objReceiver, const char* szFunctor);

	public:
		virtual void RecvReport(unsigned int uSID, qint64 nTime, IArchiveBase* iArchive) = 0;
		void SetReportDelegate(const SendReportDelegate& dgSendReport);

	protected:
		Q_INVOKABLE void ActSend(unsigned int uSID, unsigned int uVersion, MsMiddleWareBase* pBase);
		Q_INVOKABLE void ActRecv(unsigned int uSID, const qint64& nTime, MsMiddleWareBase* pBase);

	signals:
		void signal_ActRecv(unsigned int uSID, const qint64& nTime, const QSharedPointer<MsMiddleWareBase>& spBase);

	protected slots:
		void slot_ReceiverDestroy(QObject* objReceiver);

	protected:
		SendReportDelegate						m_dgSendReport;

		typedef QMap<QObject*, QVector<const char*> > MapReceiver;
		MapReceiver								m_mapReceiver;
	};

}

#endif // __RMREPORTBASE_H__
