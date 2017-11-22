#include "xml/rpcxmlreport.h"
#include "rpcreportbase.h"
#include "msmiddlewaredata.hpp"

#include <memory>
#include <QHash>
#include <QDateTime>

namespace MSRPC
{
	/*
		XmlReport格式
	{
		"report":<报告名称>
		"version":<接口版本号>
		"payload":<数据内容>
	}
	
	*/

	namespace
	{
		enum ElemText { ET_REPORT, ET_VERSION, ET_PAYLOAD };

		const char* szElemText[] =
		{
			"report",
			"version",
			"payload",
		};

	}

	typedef QHash<QLatin1String, RpcReportBase*> HsReport;

	class RpcXmlReport::Impl
	{
	public:
		HsReport hsReport;
	};

	RpcXmlReport::RpcXmlReport()
		: RpcDistributor()
		, m_pImpl(new Impl)
	{

	}

	RpcXmlReport::~RpcXmlReport()
	{
		qDeleteAll(m_pImpl->hsReport);
	}

	qint8 RpcXmlReport::GetType() const
	{
		return DT_JSON_REP;
	}

	void RpcXmlReport::ReceiveData(unsigned int uSID, QByteArray& baData)
	{
		/*
		SPtrDocument spRoot(new rapidjson::Document);
		spRoot->Parse<0>(baData.data());
		
		rapidjson::Value::ConstMemberIterator itrFind = spRoot->FindMember(szElemText[ET_REPORT]);
		if (itrFind != spRoot->MemberEnd())
		{
			HsReport::const_iterator ciFind = 
				m_pImpl->hsReport.constFind(QLatin1String(itrFind->value.GetString()));
			if (ciFind != m_pImpl->hsReport.constEnd())
			{
				unsigned int uVersion = (*spRoot)[szElemText[ET_VERSION]].GetUint();
				(*ciFind)->RecvReport(uSID, &MSRPC::IArchive_r(MSRPC::XmlIArchive(spRoot, &(*spRoot)[szElemText[ET_PAYLOAD]], uVersion)));
			}
		}
		*/
	}
	
	void RpcXmlReport::RegReportBase(RpcReportBase* pReportBase)
	{
		pReportBase->SetReportDelegate(fastdelegate::MakeDelegate(this, &RpcXmlReport::SendDate));
		m_pImpl->hsReport[QLatin1String(pReportBase->GetName())] = pReportBase;
	}

	void RpcXmlReport::SendDate(unsigned int uSID, const RpcReportBase* pBase, MsMiddleWareBase* mdData)
	{
		/*
		rapidjson::Value vValue;
		XmlOArchive json(mdData->GetVersion());
		json.SetCurNode(&vValue);
		mdData->Bale(&OArchive_r(json));

		SPtrDocument spRoot = json.GetRoot();
		spRoot->SetObject();
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_REPORT]), rapidjson::StringRef(pBase->GetName()), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_VERSION]), json.GetVersion(), spRoot->GetAllocator());
		spRoot->AddMember(rapidjson::StringRef(szElemText[ET_PAYLOAD]), *json.GetCurNode(), spRoot->GetAllocator());
		
		QByteArray baData;
		MSRPC::QByteArrayAdapter stream(baData);
		rapidjson::Writer< MSRPC::QByteArrayAdapter > writer(stream);
		spRoot->Accept(writer);

		m_dgSendData(uSID, baData, GetType());
		*/
	}

	MSRPC::RpcReportBase* RpcXmlReport::GetReportBase(const char* szReportName) const
	{
		return m_pImpl->hsReport.value(QLatin1String(szReportName), 0);
	}


}
