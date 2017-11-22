#ifndef MSMWXMLDATA_H__
#define MSMWXMLDATA_H__

#include "msserialize/xml/msxmlserialize.hpp"
#include "../msmiddlewaredata.hpp"
#include <QSharedPointer>
#include <QMetaType>


namespace MSRPC
{
	class XML
	{
	public:

	};

	typedef MSRPC::XML TSTYPE;

	template <class T>
	class MsMiddleWareData<T, TSTYPE> : public MsMiddleWareBase
	{
	public:
		MsMiddleWareData() {}
		MsMiddleWareData(const T& t)
			: spT(t) {}

		virtual bool Bale(OArchiveBase* oArchive) override
		{
			XmlOArchive& xml = static_cast<OArchive_r*>
				(oArchive)->Ref<XmlOArchive>();

			xml << spT;

			return true;
		}

		virtual bool Parse(IArchiveBase* iArchive) override
		{
			const XmlIArchive& xml = static_cast<IArchive_r*>
				(iArchive)->Ref<XmlIArchive>();

			if (xml.GetCurNode())
			{
				xml >> spT;
			}

			return true;
		}

		T spT;
	};

	template <class T>
	class MsMiddleWareData<QSharedPointer<T>, TSTYPE> : public MsMiddleWareBase
	{
	public:
		MsMiddleWareData() {}
		MsMiddleWareData(const QSharedPointer<T>& t)
			: spT(t) {}

		virtual bool Bale(OArchiveBase* oArchive)
		{
			XmlOArchive& xml = static_cast<OArchive_r*>
				(oArchive)->Ref<XmlOArchive>();

			if (spT)
			{
				xml << *spT;
			}

			return true;
		}

		virtual bool Parse(IArchiveBase* iArchive)
		{
			const XmlIArchive& xml = static_cast<IArchive_r*>
				(iArchive)->Ref<XmlIArchive>();

			spT = QSharedPointer<T>(new T);

			if (xml.GetCurNode())
			{
				xml >> *spT;
			}

			return true;
		}

		QSharedPointer<T> spT;
	};

	template <>
	class MsMiddleWareData<void, TSTYPE> : public MsMiddleWareBase
	{
	public:
		virtual bool Bale(OArchiveBase*)
		{
			return true;
		}

		virtual bool Parse(IArchiveBase*)
		{
			return true;
		}
	};
}

#endif // MSMWXMLDATA_H__
