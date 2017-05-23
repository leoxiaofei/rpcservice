#ifndef __RMJSONREQUEST_H__
#define __RMJSONREQUEST_H__

#include "../rpcdistributor.h"

namespace MSRPC
{
	class MsMiddleWareBase;
	class RpcRequestBase;

	class RPCSERVICE_EXPORT RpcJsonRequest : public RpcDistributor
	{
	public:
		RpcJsonRequest();
		~RpcJsonRequest();

		template<class T>
		T* RegRequest()
		{
			T* pRet = new T;
			RegRequestBase(pRet);
			return pRet;
		}

		template<class T>
		T* GetRequest() const
		{
			return static_cast<T*>(GetRequestBase(T::ReqName()));
		}

		enum {
			Type = DT_JSON_REQ
		};
		
	public:
		virtual qint8 GetType() const override;
		virtual void ReceiveData(unsigned int uSID, QByteArray& baData) override;

		void RegRequestBase(RpcRequestBase* pRequestBase);
		RpcRequestBase* GetRequestBase(const char* szRequestName) const;

		void SendRequestDate(unsigned int uSID, unsigned int uSequence, const RpcRequestBase* pBase, MsMiddleWareBase* mdData);
		void SendRespondDate(unsigned int uSID, unsigned int uSequence, bool bReturn, const RpcRequestBase* pBase, MsMiddleWareBase* mdData);

	private:
		class Impl;
		QScopedPointer<Impl> m_pImpl;

	};

}
#endif // __RMJSONREQUEST_H__
