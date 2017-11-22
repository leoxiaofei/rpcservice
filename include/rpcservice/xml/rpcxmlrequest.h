#ifndef RPCXMLREQUEST_H__
#define RPCXMLREQUEST_H__


#include "../rpcdistributor.h"

namespace MSRPC
{
	class MsMiddleWareBase;
	class RpcRequestBase;

	class RPCSERVICE_EXPORT RpcXmlRequest : public RpcDistributor
	{
	public:
		RpcXmlRequest();
		~RpcXmlRequest();

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
			Type = DT_XML_REQ
		};
		
	public:
		virtual qint8 GetType() const override;
		virtual void ReceiveData(unsigned int uSID, QByteArray& baData) override;

		void RegRequestBase(RpcRequestBase* pRequestBase);
		RpcRequestBase* GetRequestBase(const char* szRequestName) const;

		void SendRequestDate(unsigned int uSID, unsigned int uSequence, const RpcRequestBase* pBase, MsMiddleWareBase* mdData);
		void SendRespondDate(unsigned int uSID, unsigned int uSequence, bool bReturn, const RpcRequestBase* pBase, MsMiddleWareBase* mdData);

		void SetNs(const char* szNs);

	private:
		class Impl;
		QScopedPointer<Impl> m_pImpl;

	};

}

#endif // RPCXMLREQUEST_H__
