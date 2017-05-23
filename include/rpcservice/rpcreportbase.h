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

	protected:
		void SetReportDelegate(const SendReportDelegate& dgSendReport);

	protected:
		Q_INVOKABLE void ActSendReport(unsigned int uSID, MsMiddleWareBase* pBase);

		virtual void RecvReport(unsigned int uSID, qint64 nTime, IArchiveBase* iArchive) = 0;

		Q_INVOKABLE virtual void ActRecvReport(unsigned int uSID, const qint64& nTime, MsMiddleWareBase* pBase) = 0;

	protected:
		friend class RpcJsonReport;
		SendReportDelegate						m_dgSendReport;
	};

}

#endif // __RMREPORTBASE_H__
