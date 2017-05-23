#ifndef __MSMIDDLEWAREDATA_H__
#define __MSMIDDLEWAREDATA_H__



namespace MSRPC
{
	class IArchiveBase
	{
	public:
		virtual ~IArchiveBase() {};
	};

	class OArchiveBase
	{
	public:
		virtual ~OArchiveBase() {};
	};

	class IArchive_r : public IArchiveBase
	{
	public:
		template <class Archive>
		IArchive_r(Archive& arc)
			: m_arc(&arc) 
		{}

		template <class Archive>
		const Archive& Ref()
		{
			return *static_cast<Archive*>(m_arc);
		}

	protected:
		void* m_arc;
	};

	class OArchive_r : public OArchiveBase
	{
	public:
		template <class Archive>
		OArchive_r(Archive& arc)
			: m_arc(&arc)
		{}

		template <class Archive>
		Archive& Ref()
		{
			return *static_cast<Archive*>(m_arc);
		}

	protected:
		void* m_arc;
	};

	class MsMiddleWareBase
	{
	public:
		virtual ~MsMiddleWareBase() {};

		virtual bool Bale(OArchiveBase* oArchive) = 0;
		virtual bool Parse(IArchiveBase* iArchive) = 0;

		void SetVersion(unsigned int uVersion)
		{
			m_uVersion = uVersion;
		}

		unsigned int GetVersion()
		{
			return m_uVersion;
		}

	private:
		unsigned int m_uVersion;
	};

	template <class T, class Archive>
	class MsMiddleWareData;
}



#endif // __MSMIDDLEWAREDATA_H__