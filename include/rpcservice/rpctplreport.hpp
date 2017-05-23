#ifndef __RMTPLREPORT_H__
#define __RMTPLREPORT_H__

#include "rpcreportbase.h"
#include "msmiddlewaredata.hpp"


namespace MSRPC
{
	template<char const REPNAME[], class T, ,unsigned int VERSION = 1>
	class RpcTplReport : public RpcReportBase
	{
	public:
		void SendReport(unsigned int uSID, const QSharedPointer<T>& spT)
		{
			MsMiddleWareBase* pBase = new MsMiddleWareData<T>(spT);
			QMetaObject::invokeMethod(this, "ActSend",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, VERSION),
				Q_ARG(MsMiddleWareBase*, pBase));
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

		virtual void RecvReport(unsigned int uSID, qint64 nTime, const JsonIArchive& json) override
		{
			MsMiddleWareData<T>* pData = new MsMiddleWareData<T>();
			pData->Parse(json);

			if (m_mapReceiver.isEmpty())
			{
				ActRecv(uSID, nTime, pData);
			}
			else
			{
				for (MapReceiver::const_iterator citorRece = m_mapReceiver.constBegin();
					citorRece != m_mapReceiver.constEnd(); ++citorRece)
				{
					for (QVector<const char *>::const_iterator citorFunctor = citorRece->constBegin();
						citorFunctor != citorRece->constEnd(); ++citorFunctor)
					{
						QMetaObject::invokeMethod(citorRece.key(), *citorFunctor,
							Q_ARG(unsigned int, uSID),
							Q_ARG(qint64, nTime),
							QArgument<QSharedPointer<T> >(pData->spTName, pData->spT)
							);
					}
				}

				delete pData;
			}
		}

	};

}


#endif // __RMTPLREPORT_H__
