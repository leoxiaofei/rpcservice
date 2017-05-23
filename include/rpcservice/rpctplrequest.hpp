#ifndef RMTPLREQUEST_H__
#define RMTPLREQUEST_H__

#include "rpcrequestbase.h"
#include "rpcresponder.hpp"
#include "msmiddlewaredata.hpp"

namespace MSRPC
{
	//////////////////////////////////////////////////////////////////////////
	template<char const REQNAME[], class Q, class A = void, class TS = MSRPC::JSON, unsigned int VERSION = 1>
	class RpcTplRequest : public RpcRequestBase
	{
	public:
		typedef RpcTplRequest<REQNAME, Q, A, TS, VERSION> TPL;
		typedef Q Question;
		typedef A Answer;

		typedef fastdelegate::FastDelegate4<unsigned int, qint64, const Q&, Responder<A>& > RequestReceiver;
		typedef fastdelegate::FastDelegate5<unsigned int, unsigned int, qint64, bool, const A&> RespondReceiver;

	public:
		int SendRequest(unsigned int uSID, const Q& spT)
		{
			int uSequence = m_dgSequence();
			
			MsMiddleWareBase* pBase = new MsMiddleWareData<Q, TS>(spT);
			pBase->SetVersion(VERSION);
			
			QMetaObject::invokeMethod(this, "ActSendRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(MsMiddleWareBase*, pBase));

			return uSequence;
		}

		void SendRespond(unsigned int uSID, unsigned int uSequence, bool bReturn, const A& spT)
		{
			MsMiddleWareBase* pBase = new MsMiddleWareData<A, TS>(spT);
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pBase));
		}

		void ConnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.append(dgRequestReceiver);
		}

		void DisconnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.erase(
				std::remove(m_vecRequest.begin(), m_vecRequest.end(), dgRequestReceiver),
				m_vecRequest.end());
		}

		void ConnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.append(dgRespondReceiver);
		}

		void DisconnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.erase(
				std::remove(m_vecRespond.begin(), m_vecRespond.end(), dgRespondReceiver),
				m_vecRespond.end());
		}

	public:
		static const char* ReqName()
		{
			return REQNAME;
		}

		virtual const char* GetName() const override
		{
			return ReqName();
		}

	protected:
		virtual void RecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, IArchiveBase* iArchive) override
		{
			///��������
			MsMiddleWareData<Q, TS>* pData = new MsMiddleWareData<Q, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void RecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, IArchiveBase* iArchive) override
		{
			///��������
			MsMiddleWareData<A, TS>* pData = new MsMiddleWareData<A, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void ActRecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, MsMiddleWareBase* pBase)
		{
			Responder<A> spRespond(fastdelegate::MakeDelegate(this, &TPL::SendRespond), uSID, uSequence);
			MsMiddleWareData<Q, TS>* pData = static_cast<MsMiddleWareData<Q, TS>* >(pBase);

			for(QVector<RequestReceiver>::const_iterator citor = m_vecRequest.constBegin();
				citor != m_vecRequest.constEnd(); ++citor)
			{
				(*citor)(uSID, nTime, pData->spT, spRespond);
			}

			delete pData;
		}

		virtual void ActRecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, MsMiddleWareBase* pBase)
		{
			MsMiddleWareData<A, TS>* pData = static_cast<MsMiddleWareData<A, TS>* >(pBase);

			for(QVector<RespondReceiver>::const_iterator citor = m_vecRespond.constBegin();
				citor != m_vecRespond.constEnd(); ++citor)
			{
				(*citor)(uSID, uSequence, nTime, bReturn, pData->spT);
			}

			delete pData;
		}

	protected:
		QVector<RequestReceiver> m_vecRequest;
		QVector<RespondReceiver> m_vecRespond;
	};


	//////////////////////////////////////////////////////////////////////////
	template<char const REQNAME[], class A, class TS, unsigned int VERSION>
	class RpcTplRequest<REQNAME, void, A, TS, VERSION>: public RpcRequestBase
	{
	public:
		typedef RpcTplRequest<REQNAME, void, A, TS, VERSION> TPL;
		typedef void Question;
		typedef A Answer;

		typedef fastdelegate::FastDelegate3<unsigned int, qint64, Responder<A>& > RequestReceiver;
		typedef fastdelegate::FastDelegate5<unsigned int, unsigned int, qint64, bool, const A&> RespondReceiver;

	public:
		int SendRequest(unsigned int uSID)
		{
			int uSequence = m_dgSequence();

			MsMiddleWareBase* pBase = new MsMiddleWareData<void, TS>();
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(MsMiddleWareBase*, pBase));

			return uSequence;
		}

		void SendRespond(unsigned int uSID, unsigned int uSequence, bool bReturn, const A& spT)
		{
			MsMiddleWareBase* pBase = new MsMiddleWareData<A, TS>(spT);
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pBase));
		}

		void ConnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.append(dgRequestReceiver);
		}

		void DisconnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.erase(
				std::remove(m_vecRequest.begin(), m_vecRequest.end(), dgRequestReceiver),
				m_vecRequest.end());
		}

		void ConnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.append(dgRespondReceiver);
		}

		void DisconnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.erase(
				std::remove(m_vecRespond.begin(), m_vecRespond.end(), dgRespondReceiver),
				m_vecRespond.end());
		}

	public:
		static const char* ReqName()
		{
			return REQNAME;
		}

		virtual const char* GetName() const override
		{
			return ReqName();
		}

	protected:
		virtual void RecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, IArchiveBase* iArchive)
		{
			///��������
			MsMiddleWareData<void, TS>* pData = new MsMiddleWareData<void, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void RecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, IArchiveBase* iArchive)
		{
			///��������
			MsMiddleWareData<A, TS>* pData = new MsMiddleWareData<A, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void ActRecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, MsMiddleWareBase* pBase)
		{
			Responder<A> spRespond(fastdelegate::MakeDelegate(this, &TPL::SendRespond), uSID, uSequence);
			MsMiddleWareData<void, TS>* pData = static_cast<MsMiddleWareData<void, TS>* >(pBase);

			for(QVector<RequestReceiver>::const_iterator citor = m_vecRequest.constBegin();
				citor != m_vecRequest.constEnd(); ++citor)
			{
				(*citor)(uSID, nTime, spRespond);
			}

			delete pData;
		}

		virtual void ActRecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, MsMiddleWareBase* pBase)
		{
			MsMiddleWareData<A, TS>* pData = static_cast<MsMiddleWareData<A, TS>* >(pBase);

			for(QVector<RespondReceiver>::const_iterator citor = m_vecRespond.constBegin();
				citor != m_vecRespond.constEnd(); ++citor)
			{
				(*citor)(uSID, uSequence, nTime, bReturn, pData->spT);
			}

			delete pData;
		}

	protected:
		QVector<RequestReceiver> m_vecRequest;
		QVector<RespondReceiver> m_vecRespond;
	};


	//////////////////////////////////////////////////////////////////////////
	template<char const REQNAME[], class Q, class TS, unsigned int VERSION>
	class RpcTplRequest<REQNAME, Q, void, TS, VERSION>: public RpcRequestBase
	{
	public:
		typedef RpcTplRequest<REQNAME, Q, void, TS, VERSION> TPL;
		typedef Q Question;
		typedef void Answer;

		typedef fastdelegate::FastDelegate4<unsigned int, qint64, const Q&, Responder<void>& > RequestReceiver;
		typedef fastdelegate::FastDelegate4<unsigned int, unsigned int, qint64, bool> RespondReceiver;

	public:
		int SendRequest(unsigned int uSID, const Q& spT)
		{
			int uSequence = m_dgSequence();

			MsMiddleWareBase* pBase = new MsMiddleWareData<Q, TS>(spT);
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(MsMiddleWareBase*, pBase));

			return uSequence;
		}

		void SendRespond(unsigned int uSID, unsigned int uSequence, bool bReturn)
		{
			MsMiddleWareBase* pBase = new MsMiddleWareData<void, TS>();
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pBase));
		}

		void ConnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.append(dgRequestReceiver);
		}

		void DisconnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.erase(
				std::remove(m_vecRequest.begin(), m_vecRequest.end(), dgRequestReceiver),
				m_vecRequest.end());
		}

		void ConnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.append(dgRespondReceiver);
		}

		void DisconnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.erase(
				std::remove(m_vecRespond.begin(), m_vecRespond.end(), dgRespondReceiver),
				m_vecRespond.end());
		}

	public:
		static const char* ReqName()
		{
			return REQNAME;
		}

		virtual const char* GetName() const override
		{
			return ReqName();
		}

	protected:
		virtual void RecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, IArchiveBase* iArchive)
		{
			///��������
			MsMiddleWareData<Q, TS>* pData = new MsMiddleWareData<Q, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void RecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, IArchiveBase* iArchive)
		{
			///��������
			MsMiddleWareData<void, TS>* pData = new MsMiddleWareData<void, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void ActRecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, MsMiddleWareBase* pBase)
		{
			Responder<void> spRespond(fastdelegate::MakeDelegate(this, &TPL::SendRespond), uSID, uSequence);
			MsMiddleWareData<Q, TS>* pData = static_cast<MsMiddleWareData<Q, TS>* >(pBase);

			for(QVector<RequestReceiver>::const_iterator citor = m_vecRequest.constBegin();
				citor != m_vecRequest.constEnd(); ++citor)
			{
				(*citor)(uSID, nTime, pData->spT, spRespond);
			}

			delete pData;
		}

		virtual void ActRecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, MsMiddleWareBase* pBase)
		{
			MsMiddleWareData<void, TS>* pData = static_cast<MsMiddleWareData<void, TS>* >(pBase);

			for(QVector<RespondReceiver>::const_iterator citor = m_vecRespond.constBegin();
				citor != m_vecRespond.constEnd(); ++citor)
			{
				(*citor)(uSID, uSequence, nTime, bReturn);
			}

			delete pData;
		}

	protected:
		QVector<RequestReceiver> m_vecRequest;
		QVector<RespondReceiver> m_vecRespond;
	};


	//////////////////////////////////////////////////////////////////////////
	template<char const REQNAME[], class TS, unsigned int VERSION>
	class RpcTplRequest<REQNAME, void, void, TS, VERSION>: public RpcRequestBase
	{
	public:
		typedef RpcTplRequest<REQNAME, void, void, TS, VERSION> TPL;
		typedef void Question;
		typedef void Answer;

		typedef fastdelegate::FastDelegate3<unsigned int, qint64, Responder<void>& > RequestReceiver;
		typedef fastdelegate::FastDelegate4<unsigned int, unsigned int, qint64, bool> RespondReceiver;

	public:
		int SendRequest(unsigned int uSID)
		{
			int uSequence = m_dgSequence();

			MsMiddleWareBase* pBase = new MsMiddleWareData<void, TS>();
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(MsMiddleWareBase*, pBase));

			return uSequence;
		}

		void SendRespond(unsigned int uSID, unsigned int uSequence, bool bReturn)
		{
			MsMiddleWareBase* pBase = new MsMiddleWareData<void, TS>();
			pBase->SetVersion(VERSION);

			QMetaObject::invokeMethod(this, "ActSendRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pBase));
		}

		void ConnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.append(dgRequestReceiver);
		}

		void DisconnectRequestReceiver(const RequestReceiver& dgRequestReceiver)
		{
			m_vecRequest.erase(
				std::remove(m_vecRequest.begin(), m_vecRequest.end(), dgRequestReceiver),
				m_vecRequest.end());
		}

		void ConnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.append(dgRespondReceiver);
		}

		void DisconnectRespondReceiver(const RespondReceiver& dgRespondReceiver)
		{
			m_vecRespond.erase(
				std::remove(m_vecRespond.begin(), m_vecRespond.end(), dgRespondReceiver),
				m_vecRespond.end());
		}

	public:
		static const char* ReqName()
		{
			return REQNAME;
		}

		virtual const char* GetName() const override
		{
			return ReqName();
		}

	protected:
		virtual void RecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, IArchiveBase* iArchive)
		{
			///��������
			MsMiddleWareData<void, TS>* pData = new MsMiddleWareData<void, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRequest",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void RecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, IArchiveBase* iArchive)
		{
			///��������
			MsMiddleWareData<void, TS>* pData = new MsMiddleWareData<void, TS>();
			pData->Parse(iArchive);

			///�л��߳�
			QMetaObject::invokeMethod(this, "ActRecvRespond",
				Q_ARG(unsigned int, uSID),
				Q_ARG(unsigned int, uSequence),
				Q_ARG(qint64, nTime),
				Q_ARG(bool, bReturn),
				Q_ARG(MsMiddleWareBase*, pData));
		}

		virtual void ActRecvRequest(unsigned int uSID, unsigned int uSequence, const qint64& nTime, MsMiddleWareBase* pBase)
		{
			Responder<void> spRespond(fastdelegate::MakeDelegate(this, &TPL::SendRespond), uSID, uSequence);
			MsMiddleWareData<void, TS>* pData = static_cast<MsMiddleWareData<void, TS>* >(pBase);

			for(QVector<RequestReceiver>::const_iterator citor = m_vecRequest.constBegin();
				citor != m_vecRequest.constEnd(); ++citor)
			{
				(*citor)(uSID, nTime, spRespond);
			}

			delete pData;
		}

		virtual void ActRecvRespond(unsigned int uSID, unsigned int uSequence, const qint64& nTime, bool bReturn, MsMiddleWareBase* pBase)
		{
			MsMiddleWareData<void, TS>* pData = static_cast<MsMiddleWareData<void, TS>* >(pBase);

			for(QVector<RespondReceiver>::const_iterator citor = m_vecRespond.constBegin();
				citor != m_vecRespond.constEnd(); ++citor)
			{
				(*citor)(uSID, uSequence, nTime, bReturn);
			}

			delete pData;
		}

	protected:
		QVector<RequestReceiver> m_vecRequest;
		QVector<RespondReceiver> m_vecRespond;
	};

}




#endif // RMTPLREQUEST_H__
