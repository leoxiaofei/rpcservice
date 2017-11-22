#include "xml/rpcxmlrequest.h"
#include "rpcrequestbase.h"
#include "mstools/idCreater.hpp"
#include "msmiddlewaredata.hpp"
#include "msserialize/xml/msxmlserialize.hpp"
#include "msserialize/xml/rapidxmlaid.hpp"

#include <memory>
#include <QHash>


namespace MSRPC
{
	/*
	XmlRequest请求格式
	{
	"request":<请求名称>
	"version":<接口版本号>
	"time":<Unix时间戳毫秒精度>
	"sequence":<命令序号>
	"payload":<数据内容（可选）>
	}

	回复格式
	{
	"respond":<请求名称>
	"version":<接口版本号>
	"sequence":<命令序号>
	"return":<true/false>
	"payload":<数据内容（可选）>
	}

	*/

	namespace
	{
		enum ElemText { ET_MSG, ET_EXEC };

		const char* szElemText[] = 
		{
			"Msg",
			"Exec",
		};

		enum AttrText { AT_NS, AT_VER, AT_TYPE, AT_SEQ, AT_CMD, AT_RET };

		const char* szAttrText[] = 
		{
			"ns",
			"ver",
			"type",
			"seq",
			"cmd",
			"ret",
		};

		enum MsgType { MT_REQUEST, MT_RESPOND };

		const char* szMsgType[] = { "request", "respond" };
		
	}

	typedef QHash<QLatin1String, RpcRequestBase*> HsRequest;

	class RpcXmlRequest::Impl
	{
	public:
		Impl() : icSequence(0) {}

		HsRequest hsRequest;
		IdCreater<unsigned int> icSequence;
		const char* szNs;
	};

	RpcXmlRequest::RpcXmlRequest()
		: RpcDistributor()
		, m_pImpl(new Impl)
	{
		m_pImpl->szNs = "MSRPC";
	}

	RpcXmlRequest::~RpcXmlRequest()
	{
		qDeleteAll(m_pImpl->hsRequest);
	}

	qint8 RpcXmlRequest::GetType() const
	{
		return RpcXmlRequest::Type;
	}

	void RpcXmlRequest::ReceiveData(unsigned int uSID, QByteArray& baData)
	{
		do 
		{
			RapidNode rnRoot = RapidNode::ParseRoot(baData.data());
			if (!rnRoot)
			{
				break;
			}

			RapidNode rnMsg = rnRoot.FirstSubElement(szElemText[ET_MSG]);
			if (!rnMsg)
			{
				break;
			}

			const char* strMsgType = rnMsg.Attribute(szAttrText[AT_TYPE], "");
			/*
			XmlRequest请求格式
			{

			}
			*/
			if (strcmp(strMsgType, szMsgType[MT_REQUEST]) == 0)
			{
				if(RapidNode rnExec = rnMsg.FirstSubElement(szElemText[ET_EXEC]))
				{
					HsRequest::const_iterator ciFind =
						m_pImpl->hsRequest.constFind(QLatin1String(rnExec.Attribute(szAttrText[AT_CMD])));
					if (ciFind != m_pImpl->hsRequest.constEnd())
					{
						unsigned int uVersion = rnMsg.Attribute(szAttrText[AT_VER], 1U) ;
						unsigned int uSequence = rnMsg.Attribute(szAttrText[AT_SEQ], 1U) ;

						(*ciFind)->RecvRequest(uSID, uSequence, &IArchive_r(XmlIArchive(rnExec.GetDoc(), 
							rnExec.GetCurNode(), uVersion)));
					}
				}
				break;
			}

			/*
			回复格式
			{

			}
			*/
			if (strcmp(strMsgType, szMsgType[MT_RESPOND]) == 0)
			{
				if(RapidNode rnExec = rnMsg.FirstSubElement(szElemText[ET_EXEC]))
				{
					HsRequest::const_iterator ciFind =
						m_pImpl->hsRequest.constFind(QLatin1String(rnExec.Attribute(szAttrText[AT_CMD])));
					if (ciFind != m_pImpl->hsRequest.constEnd())
					{
						unsigned int uVersion = rnMsg.Attribute(szAttrText[AT_VER], 1U) ;
						unsigned int uSequence = rnMsg.Attribute(szAttrText[AT_SEQ], 1U) ;
						bool bReturn = rnExec.Attribute(szAttrText[AT_RET], false) ;

						(*ciFind)->RecvRespond(uSID, uSequence, bReturn, &IArchive_r(XmlIArchive(rnExec.GetDoc(), 
							rnExec.GetCurNode(), uVersion)));
					}
				}
				break;
			}

		} while (0);
	}

	void RpcXmlRequest::RegRequestBase(RpcRequestBase* pRequestBase)
	{
		pRequestBase->SetSequenceDelegate(fastdelegate::MakeDelegate(
			&m_pImpl->icSequence, &IdCreater<unsigned int>::operator ()));
		pRequestBase->SetRequestDelegate(fastdelegate::MakeDelegate(this, &RpcXmlRequest::SendRequestDate));
		pRequestBase->SetRespondDelegate(fastdelegate::MakeDelegate(this, &RpcXmlRequest::SendRespondDate));
		m_pImpl->hsRequest[QLatin1String(pRequestBase->GetName())] = pRequestBase;
	}

	void RpcXmlRequest::SendRequestDate(unsigned int uSID, unsigned int uSequence, const RpcRequestBase* pBase, MsMiddleWareBase* mdData)
	{
		RapidNode rnRoot = RapidNode::CreateRoot();

		RapidNode rnMsg = rnRoot.CreateElement(szElemText[ET_MSG]);
		rnMsg.SetAttribute(szAttrText[AT_NS], m_pImpl->szNs);
		rnMsg.SetAttribute(szAttrText[AT_VER], mdData->GetVersion());
		rnMsg.SetAttribute(szAttrText[AT_TYPE], szMsgType[MT_REQUEST]);
		rnMsg.SetAttribute(szAttrText[AT_SEQ], uSequence);

		RapidNode rnExec = rnMsg.CreateElement(szElemText[ET_EXEC]);
		rnExec.SetAttribute(szAttrText[AT_CMD], pBase->GetName());

		XmlOArchive xml(rnExec.GetDoc(), rnExec.GetCurNode(), mdData->GetVersion());
		mdData->Bale(&OArchive_r(xml));

		QByteArray baData;
		rnRoot.Print(baData);

		m_dgSendData(uSID, baData, GetType());
	}

	void RpcXmlRequest::SendRespondDate(unsigned int uSID, unsigned int uSequence, bool bReturn, const RpcRequestBase* pBase, MsMiddleWareBase* mdData)
	{
		RapidNode rnRoot = RapidNode::CreateRoot();

		RapidNode rnMsg = rnRoot.CreateElement(szElemText[ET_MSG]);
		rnMsg.SetAttribute(szAttrText[AT_NS], m_pImpl->szNs);
		rnMsg.SetAttribute(szAttrText[AT_VER], mdData->GetVersion());
		rnMsg.SetAttribute(szAttrText[AT_TYPE], szMsgType[MT_RESPOND]);
		rnMsg.SetAttribute(szAttrText[AT_SEQ], uSequence);

		RapidNode rnExec = rnMsg.CreateElement(szElemText[ET_EXEC]);
		rnExec.SetAttribute(szAttrText[AT_CMD], pBase->GetName());
		rnExec.SetAttribute(szAttrText[AT_RET], bReturn);

		XmlOArchive xml(rnExec.GetDoc(), rnExec.GetCurNode(), mdData->GetVersion());
		mdData->Bale(&OArchive_r(xml));

		QByteArray baData;
		rnRoot.Print(baData);

		m_dgSendData(uSID, baData, GetType());
	}

	void RpcXmlRequest::SetNs(const char* szNs)
	{
		m_pImpl->szNs = szNs;
	}

	MSRPC::RpcRequestBase* RpcXmlRequest::GetRequestBase(const char* szRequestName) const
	{
		return m_pImpl->hsRequest.value(QLatin1String(szRequestName), 0);
	}

}