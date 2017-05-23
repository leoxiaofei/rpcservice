#ifndef __MSMWJSONDATA_H__
#define __MSMWJSONDATA_H__

#include "rapidjson/msjson/msjsonserialize.hpp"
#include "../msmiddlewaredata.hpp"
#include <QSharedPointer>
#include <QMetaType>

namespace MSRPC
{
	class JSON
	{
	public:

	};

	template <class T>
	class MsMiddleWareData<T, MSRPC::JSON> : public MsMiddleWareBase
	{
	public:
		MsMiddleWareData() {}
		MsMiddleWareData(const T& t)
			: spT(t) {}

		virtual bool Bale(OArchiveBase* oArchive) override
		{
			JsonOArchive& json = static_cast<OArchive_r*>
				(oArchive)->Ref<JsonOArchive>();

			json << spT;

			return true;
		}

		virtual bool Parse(IArchiveBase* iArchive) override
		{
			const JsonIArchive& json = static_cast<IArchive_r*>
				(iArchive)->Ref<JsonIArchive>();

			if (json.GetCurNode() && !json.GetCurNode()->IsNull())
			{
				json >> spT;
			}

			return true;
		}

		T spT;
	};

	template <class T>
	class MsMiddleWareData<QSharedPointer<T>, MSRPC::JSON> : public MsMiddleWareBase
	{
	public:
		MsMiddleWareData() {}
		MsMiddleWareData(const QSharedPointer<T>& t)
			: spT(t) {}

		virtual bool Bale(OArchiveBase* oArchive)
		{
			JsonOArchive& json = static_cast<OArchive_r*>
				(oArchive)->Ref<JsonOArchive>();

			if (spT)
			{
				json << *spT;
			}

			return true;
		}

		virtual bool Parse(IArchiveBase* iArchive)
		{
			const JsonIArchive& json = static_cast<IArchive_r*>
				(iArchive)->Ref<JsonIArchive>();

			spT = QSharedPointer<T>(new T);

			if (json.GetCurNode() && !json.GetCurNode()->IsNull())
			{
				json >> *spT;
			}

			return true;
		}

		QSharedPointer<T> spT;
	};

	template <>
	class MsMiddleWareData<void, MSRPC::JSON> : public MsMiddleWareBase
	{
	public:
		virtual bool Bale(OArchiveBase* oArchive)
		{
			return true;
		}

		virtual bool Parse(IArchiveBase* iArchive)
		{
			return true;
		}
	};
}


#endif // __MSMWJSONDATA_H__
