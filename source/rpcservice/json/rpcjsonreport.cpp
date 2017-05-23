#include "json/rpcjsonreport.h"
#include "rpcreportbase.h"
#include "rapidjson/msjson/msjsonserialize.hpp"
#include "rapidjson/msjson/msjsonadapter.hpp"
#include "msmiddlewaredata.hpp"

#include <memory>
#include <QHash>
#include <QDateTime>

namespace MSRPC
{
	/*
		JsonReport格式
	{
		"report":<报告名称>
		"version":<接口版本号>
		"time":<Unix时间戳毫秒精度>
		"payload":<数据内容>
	}
	
	*/

	namespace
	{
		enum ElemText { ET_REPORT, ET_VERSION, ET_TIME, ET_PAYLOAD };

		const char* szElemText[] =
		{
			"report",
			"version",
			"time",
			"payload",
		};

	}

	typedef QHash<QLatin1String, RpcReportBase*> HsReport;

	class RpcJsonReport::Impl
	{
	public:
		HsReport hsReport;
	};

	RpcJsonReport::RpcJsonReport()
		: RpcDistributor()
		, m_pImpl(new Impl)
	{

	}

	RpcJsonReport::~RpcJsonReport()
	{
		qDeleteAll(m_pImpl->hsReport);
	}

	qint8 RpcJsonReport::GetType() const
	{
		return DT_JSON_REP;
	}

	void RpcJsonReport::ReceiveData(unsigned int uSID, QByteArray& baData)
	{
		SPtrDocument spRoot(new rapidjson::Document);
		spRoot->Parse<0>(baData.data());
		
		rapidjson::Value::ConstMemberIterator itrFind = spRoot->FindMember(szElemText[ET_REPORT]);
		if (itrFind != spRoot->MemberEnd())
		{
			HsReport::const_iterator ciFind = 
				m_pImpl->hsReport.constFind(QLatin1String(itrFind->value.GetString()));
			if (ciFind != m_pImpl->hsReport.constEnd())
			{
				qint64 nTime = (*spRoot)[szElemText[ET_TIME]].GetInt64();
				unsigned int uVersion = (*spRoot)[szElemText[ET_VERSION]].GetUint();
				(*ciFind)->RecvReport(uSID, nTime, &MSRPC::IArchive_r(MSRPC::JsonIArchive(spRoot, &(*spRoot)[szElemText[ET_PAYLOAD]], uVersion)));
			}
		}
	}
	
	void RpcJsonReport::RegReportBase(RpcReportBase* pReportBase)
	{
		pReportBase->SetReportDelegate(fastdelegate::MakeDelegate(this, &RpcJsonReport::SendDate));
		m_pImpl->hsReport[QLatin1String(pReportBase->GetName())] = pReportBase;
	}

	void RpcJsonReport::SendDate(unsigned int uSID, const RpcReportBase* pBase, MsMiddleWareBase* mdData)
	{
		rapidjson::Value vValue;
		JsonOArchive json(mdData->GetVersion());
		json.SetCurNode(&vValue);
		mdData->Bale(&OArchive_r(json));

		SPtrDocument spRoot = json.GetRoot();
		spRoot->SetObject();
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_REPORT]), rapidjson::StringRef(pBase->GetName()), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_VERSION]), json.GetVersion(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_TIME]), QDateTime::currentMSecsSinceEpoch(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_PAYLOAD]), *json.GetCurNode(), spRoot->GetAllocator());
		
		QByteArray baData;
		MSRPC::QByteArrayAdapter stream(baData);
		rapidjson::Writer< MSRPC::QByteArrayAdapter > writer(stream);
		spRoot->Accept(writer);

		m_dgSendData(uSID, baData, GetType());
	}

	MSRPC::RpcReportBase* RpcJsonReport::GetReportBase(const char* szReportName) const
	{
		return m_pImpl->hsReport.value(QLatin1String(szReportName), 0);
	}


}
