#ifndef __RMTPLREPORT_H__
#define __RMTPLREPORT_H__

#include "rpcreportbase.h"
#include "msmiddlewaredata.hpp"


namespace MSRPC
{
	template<char const REPNAME[], class T = void, class TS = MSRPC::JSON, unsigned int VERSION = 1>
	class RpcTplReport : public RpcReportBase
	{
	public:
		typedef RpcTplReport<REPNAME, T, TS, VERSION> TPL;
		typedef T Report;

		typedef fastdelegate::FastDelegate3<unsigned int, qint64, const T&> ReportReceiver;

	public:
		void SendReport(unsigned int uSID, const T& spT)
		{
			MsMiddleWareBase* pBase = new MsMiddleWareData<T, TS>(spT);
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendReport",
				Q_ARG(unsigned int, uSID),
				Q_ARG(MsMiddleWareBase*, pBase));
		}

		void ConnectReportReceiver(const ReportReceiver& dgReportReceiver)
		{
			m_vecReport.append(dgReportReceiver);
		}

		void DisconnectReportReceiver(const ReportReceiver& dgReportReceiver)
		{
			m_vecReport.erase(
				std::remove(m_vecReport.begin(), m_vecReport.end(), dgReportReceiver),
				m_vecReport.end());
		}

	public:
		static const char* RepName()
		{
			return REPNAME;
		}

		virtual const char* GetName() const override
		{
			return RepName();
		}

		virtual void RecvReport(unsigned int uSID, qint64 nTime, IArchiveBase* iArchive) override
		{
			///解析数据
			MsMiddleWareData<T, TS>* pData = new MsMiddleWareData<T, TS>();
			pData->Parse(iArchive);

			///切换线程
			QMetaObject::invokeMethod(this, "ActRecvReport",
				Q_ARG(unsigned int, uSID),
				Q_ARG(qint64, nTime),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void ActRecvReport(unsigned int uSID, const qint64& nTime, MsMiddleWareBase* pBase) override
		{
			MsMiddleWareData<T, TS>* pData = static_cast<MsMiddleWareData<T, TS>*>(pBase);

			for (QVector<ReportReceiver>::const_iterator citor = m_vecReport.constBegin();
				citor != m_vecReport.constEnd(); ++citor)
			{
				(*citor)(uSID, nTime, pData->spT);
			}

			delete pData;
		}

	protected:
		QVector<ReportReceiver> m_vecReport;

	};

}


#endif // __RMTPLREPORT_H__
