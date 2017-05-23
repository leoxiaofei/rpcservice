#ifndef __RMREQUESTBASE_H__
#define __RMREQUESTBASE_H__

#include "rpcservice_global.h"

#include "mstools/FastDelegate.h"
#include <QObject>
#include <QMap>
#include <QVector>
#include <QSharedPointer>

namespace MSRPC
{
	class IArchiveBase;
	class MsMiddleWareBase;

	typedef fastdelegate::FastDelegate0<unsigned int> SequenceDelegate;
	typedef fastdelegate::FastDelegate4<unsigned int, unsigned int, 
		const class RpcRequestBase*, MsMiddleWareBase*> SendRequestDelegate;
	typedef fastdelegate::FastDelegate5<unsigned int, unsigned int, 
		bool, const class RpcRequestBase*, MsMiddleWareBase*> SendRespondDelegate;

class RPCSERVICE_EXPORT RpcRequestBase : public QObject
{
	Q_OBJECT
public:
	virtual const char* GetName() const = 0;

protected:
	void SetRequestDelegate(const SendRequestDelegate& dgSendRequest);
	void SetRespondDelegate(const SendRespondDelegate& dgSendRespond);
	void SetSequenceDelegate(const SequenceDelegate& dgSequence);

protected:
	Q_INVOKABLE void ActSendRequest(unsigned int uSID, unsigned int uSequence, MsMiddleWareBase* pBase);
	Q_INVOKABLE void ActSendRespond(unsigned int uSID, unsigned int uSequence, bool bReturn, MsMiddleWareBase* pBase);

	virtual void RecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, IArchiveBase* iArchive) = 0;
	virtual void RecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, IArchiveBase* iArchive) = 0;

	Q_INVOKABLE virtual void ActRecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, MsMiddleWareBase* pBase) = 0;
	Q_INVOKABLE virtual void ActRecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, MsMiddleWareBase* pBase) = 0;

protected:
	friend class RpcJsonRequest;

	SendRequestDelegate m_dgSendRequest;
	SendRespondDelegate m_dgSendRespond;
	SequenceDelegate m_dgSequence;
};


}

#endif // __RMREQUESTBASE_H__
