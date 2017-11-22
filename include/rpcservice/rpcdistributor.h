#ifndef __RMDISTRIBUTOR_H__
#define __RMDISTRIBUTOR_H__

#include "rpcservice_global.h"
#include "mstools/FastDelegate.h"
#include <QObject>

class QByteArray;

namespace MSRPC
{
	typedef fastdelegate::FastDelegate3<unsigned int, const QByteArray&, quint8> SendDataDelegate;

class RPCSERVICE_EXPORT RpcDistributor
{
public:
	virtual ~RpcDistributor();

	enum DisType { DT_JSON_REQ = 1, DT_JSON_REP, DT_XML_REQ, DT_XML_REP };
	virtual qint8 GetType() const = 0;

public:
	void SetSendDataDelegate(const SendDataDelegate& dgSendData);
	virtual void ReceiveData(unsigned int uSID, QByteArray& baData) = 0;
	virtual void SendData(unsigned int uSID, const QByteArray& baData);

protected:
	SendDataDelegate m_dgSendData;
};

}

#endif // __RMDISTRIBUTOR_H__
