#include "json/rpcjsonrequest.h"
#include "rapidjson/msjson/msjsonserialize.hpp"
#include "rapidjson/msjson/msjsonadapter.hpp"
#include "rpcrequestbase.h"
#include "mstools/idCreater.hpp"
#include "msmiddlewaredata.hpp"

#include <memory>
#include <QHash>
#include <QDateTime>

namespace MSRPC
{
	/*
	JsonRequest请求格式
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
	"time":<Unix时间戳毫秒精度>
	"sequence":<命令序号>
	"return":<true/false>
	"payload":<数据内容（可选）>
	}

	*/

	namespace
	{
		enum ElemText { ET_REQUEST, ET_RESPOND, ET_VERSION, ET_TIME, ET_SEQUENCE, ET_RETURN, ET_PAYLOAD };

		const char* szElemText[] =
		{
			"request",
			"respond",
			"version",
			"time",
			"sequence",
			"return",
			"payload",
		};
	}

	typedef QHash<QLatin1String, RpcRequestBase*> HsRequest;

	class RpcJsonRequest::Impl
	{
	public:
		Impl() : icSequence(0) {}

		HsRequest hsRequest;
		IdCreater<unsigned int> icSequence;
	};

	RpcJsonRequest::RpcJsonRequest()
		: RpcDistributor()
		, m_pImpl(new Impl)
	{

	}

	RpcJsonRequest::~RpcJsonRequest()
	{
		qDeleteAll(m_pImpl->hsRequest);
	}

	qint8 RpcJsonRequest::GetType() const
	{
		return DT_JSON_REQ;
	}

	void RpcJsonRequest::ReceiveData(unsigned int uSID, QByteArray& baData)
	{
		SPtrDocument spRoot(new rapidjson::Document);
		spRoot->Parse<0>(baData.data());
		
		do 
		{
			/*
			JsonRequest请求格式
			{
			"request":<请求名称>
			"version":<接口版本号>
			"time":<Unix时间戳毫秒精度>
			"sequence":<命令序号>
			"payload":<数据内容（可选）>
			}
			*/

			rapidjson::Value::ConstMemberIterator itrFind = spRoot->FindMember(szElemText[ET_REQUEST]);
			if (itrFind != spRoot->MemberEnd())
			{
				HsRequest::const_iterator ciFind =
					m_pImpl->hsRequest.constFind(QLatin1String(itrFind->value.GetString()));
				if (ciFind != m_pImpl->hsRequest.constEnd())
				{
					qint64 nTime = (*spRoot)[szElemText[ET_TIME]].GetInt64();
					unsigned int uVersion = (*spRoot)[szElemText[ET_VERSION]].GetUint();
					unsigned int uSequence = (*spRoot)[szElemText[ET_SEQUENCE]].GetUint();

					rapidjson::Value::MemberIterator itrPayload = spRoot->FindMember(szElemText[ET_PAYLOAD]);

					(*ciFind)->RecvRequest(uSID, uSequence, nTime, &IArchive_r(JsonIArchive(spRoot, 
						(itrPayload != spRoot->MemberEnd() ? &itrPayload->value : 0), uVersion)));
				}
				break;
			}

			/*
			回复格式
			{
			"respond":<请求名称>
			"version":<接口版本号>
			"time":<Unix时间戳毫秒精度>
			"sequence":<命令序号>
			"return":<true/false>
			"payload":<数据内容（可选）>
			}
			*/
			itrFind = spRoot->FindMember(szElemText[ET_RESPOND]);
			if (itrFind != spRoot->MemberEnd())
			{
				HsRequest::const_iterator ciFind =
					m_pImpl->hsRequest.constFind(QLatin1String(itrFind->value.GetString()));
				if (ciFind != m_pImpl->hsRequest.constEnd())
				{
					qint64 nTime = (*spRoot)[szElemText[ET_TIME]].GetInt64();
					unsigned int uVersion = (*spRoot)[szElemText[ET_VERSION]].GetUint();
					unsigned int uSequence = (*spRoot)[szElemText[ET_SEQUENCE]].GetUint();
					bool bReturn = (*spRoot)[szElemText[ET_RETURN]].GetBool();

					rapidjson::Value::MemberIterator itrPayload = spRoot->FindMember(szElemText[ET_PAYLOAD]);

					(*ciFind)->RecvRespond(uSID, uSequence, nTime, bReturn, &MSRPC::IArchive_r(JsonIArchive(spRoot,
						(itrPayload != spRoot->MemberEnd() ? &itrPayload->value : 0), uVersion)));
				}
			}

		} while (0);
	}

	void RpcJsonRequest::RegRequestBase(RpcRequestBase* pRequestBase)
	{
		pRequestBase->SetSequenceDelegate(fastdelegate::MakeDelegate(
			&m_pImpl->icSequence, &IdCreater<unsigned int>::operator ()));
		pRequestBase->SetRequestDelegate(fastdelegate::MakeDelegate(this, &RpcJsonRequest::SendRequestDate));
		pRequestBase->SetRespondDelegate(fastdelegate::MakeDelegate(this, &RpcJsonRequest::SendRespondDate));
		m_pImpl->hsRequest[QLatin1String(pRequestBase->GetName())] = pRequestBase;
	}

	void RpcJsonRequest::SendRequestDate(unsigned int uSID, unsigned int uSequence, const RpcRequestBase* pBase, MsMiddleWareBase* mdData)
	{
		rapidjson::Value vValue;
		JsonOArchive json(mdData->GetVersion());
		json.SetCurNode(&vValue);
		mdData->Bale(&OArchive_r(json));

		SPtrDocument spRoot = json.GetRoot();
		spRoot->SetObject();
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_REQUEST]), rapidjson::StringRef(pBase->GetName()), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_VERSION]), json.GetVersion(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_TIME]), QDateTime::currentMSecsSinceEpoch(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_SEQUENCE]), uSequence, spRoot->GetAllocator());

		if (json.GetCurNode())
		{
			spRoot->AddMember(rapidjson::StringRef(szElemText[ET_PAYLOAD]), *json.GetCurNode(), spRoot->GetAllocator());
		}

		QByteArray baData;
		MSRPC::QByteArrayAdapter stream(baData);
		rapidjson::Writer< MSRPC::QByteArrayAdapter > writer(stream);
		spRoot->Accept(writer);

		m_dgSendData(uSID, baData, GetType());
	}

	void RpcJsonRequest::SendRespondDate(unsigned int uSID, unsigned int uSequence, bool bReturn, const RpcRequestBase* pBase, MsMiddleWareBase* mdData)
	{
		rapidjson::Value vValue;
		JsonOArchive json(mdData->GetVersion());
		json.SetCurNode(&vValue);
		mdData->Bale(&OArchive_r(json));


		SPtrDocument spRoot = json.GetRoot();
		spRoot->SetObject();
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_RESPOND]), rapidjson::StringRef(pBase->GetName()), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_VERSION]), json.GetVersion(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_TIME]), QDateTime::currentMSecsSinceEpoch(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_SEQUENCE]), uSequence, spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_RETURN]), bReturn, spRoot->GetAllocator());

		if (json.GetCurNode())
		{
			spRoot->AddMember(rapidjson::StringRef(szElemText[ET_PAYLOAD]), *json.GetCurNode(), spRoot->GetAllocator());
		}

		QByteArray baData;
		MSRPC::QByteArrayAdapter stream(baData);
		rapidjson::Writer< MSRPC::QByteArrayAdapter > writer(stream);
		spRoot->Accept(writer);

		m_dgSendData(uSID, baData, GetType());
	}

	MSRPC::RpcRequestBase* RpcJsonRequest::GetRequestBase(const char* szRequestName) const
	{
		return m_pImpl->hsRequest.value(QLatin1String(szRequestName), 0);
	}

}