#include "rpcreportbase.h"
#include "rapidjson/msjson/msjsonserialize.hpp"
#include "msmiddlewaredata.hpp"


namespace MSRPC
{
	void RpcReportBase::SetReportDelegate(const SendReportDelegate& dgSendReport)
	{
		m_dgSendReport = dgSendReport;
	}

	Q_INVOKABLE void RpcReportBase::ActSendReport(unsigned int uSID, MsMiddleWareBase* pBase)
	{
		m_dgSendReport(uSID, this, pBase);
		delete pBase;
	}

}

