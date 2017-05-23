#include "rpcdistributor.h"

namespace MSRPC
{
	RpcDistributor::~RpcDistributor()
	{

	}

	void RpcDistributor::SendData(unsigned int uSID, const QByteArray& baData)
	{
		m_dgSendData(uSID, baData, GetType());
	}

	void RpcDistributor::SetSendDataDelegate(const SendDataDelegate& dgSendData)
	{
		m_dgSendData = dgSendData;
	}

}
