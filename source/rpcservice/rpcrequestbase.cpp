#include "rpcrequestbase.h"
#include "msmiddlewaredata.hpp"

namespace MSRPC
{
	void RpcRequestBase::SetRequestDelegate(const SendRequestDelegate& dgSendRequest)
	{
		m_dgSendRequest = dgSendRequest;
	}

	void RpcRequestBase::SetRespondDelegate(const SendRespondDelegate& dgSendRespond)
	{
		m_dgSendRespond = dgSendRespond;
	}

	void RpcRequestBase::SetSequenceDelegate(const SequenceDelegate& dgSequence)
	{
		m_dgSequence = dgSequence;
	}

	void RpcRequestBase::ActSendRequest(unsigned int uSID, unsigned int uSequence, MsMiddleWareBase* pBase)
	{
		m_dgSendRequest(uSID, uSequence, this, pBase);
		delete pBase;
	}

	void RpcRequestBase::ActSendRespond(unsigned int uSID, unsigned int uSequence, bool bReturn, MsMiddleWareBase* pBase)
	{
		m_dgSendRespond(uSID, uSequence, bReturn, this, pBase);
		delete pBase;
	}

}