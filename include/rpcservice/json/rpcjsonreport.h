#ifndef __RPCJSONREPORT_H__
#define __RPCJSONREPORT_H__


#include "../rpcdistributor.h"

namespace MSRPC
{
	class MsMiddleWareBase;
	class RpcReportBase;

class RPCSERVICE_EXPORT RpcJsonReport : public RpcDistributor
{
public:
	RpcJsonReport();
	~RpcJsonReport();

	template<class T>
	T* RegReport()
	{
		T* pRet = new T;
		RegReportBase(pRet);
		return pRet;
	}

	template<class T>
	T* GetReport() const
	{
		return static_cast<T*>(GetReportBase(T::RepName()));
	}

	enum {
		Type = DT_JSON_REP
	};

public:
	virtual qint8 GetType() const override;
	virtual void ReceiveData(unsigned int uSID, QByteArray& baData) override;

	void RegReportBase(RpcReportBase* pReportBase);
	RpcReportBase* GetReportBase(const char* szReportName) const;
	void SendDate(unsigned int uSID, const RpcReportBase* pBase, MsMiddleWareBase* mdData);

private:
	class Impl;
	QScopedPointer<Impl> m_pImpl;

};

}


#endif // __RPCJSONREPORT_H__
