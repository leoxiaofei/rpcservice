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

	public:
		void SetReportDelegate(const SendReportDelegate& dgSendReport);

		virtual void RecvReport(unsigned int uSID, IArchiveBase* iArchive) = 0;

	protected:
		Q_INVOKABLE void ActSendReport(unsigned int uSID, MsMiddleWareBase* pBase);
		Q_INVOKABLE virtual void ActRecvReport(unsigned int uSID, MsMiddleWareBase* pBase) = 0;

	protected:
		SendReportDelegate						m_dgSendReport;
	};

}

#endif // __RMREPORTBASE_H__
