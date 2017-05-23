#ifndef __RMJSONREPORT_H__
#define __RMJSONREPORT_H__

#include "../rpcdistributor.h"

namespace MSRPC
{
	class MsMiddleWareBase;
	class RpcReportBase;

class RPCSERVICE_EXPORT RmJsonReport : public RpcDistributor
{
public:
	RmJsonReport();
	~RmJsonReport();

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
		return GetReportBase(T::RepName());
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

#endif // __RMJSONREPORT_H__
