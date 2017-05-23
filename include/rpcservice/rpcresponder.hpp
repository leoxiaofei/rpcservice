#ifndef RMRESPONDER_H__
#define RMRESPONDER_H__

#include "mstools/FastDelegate.h"

namespace MSRPC
{
	template<class S>
	class Responder
	{
	public:
		typedef fastdelegate::FastDelegate4<unsigned int, unsigned int,
			bool, const S&> SendRespondDelegate;

		SendRespondDelegate dgSendRespond;
		unsigned int uSID;
		unsigned int uSequence;
		bool bHasRet;
		static const char* spTName;

	public:
		Responder(const SendRespondDelegate& send, unsigned int sid, unsigned int sequence)
			: dgSendRespond(send)
			, uSID(sid)
			, uSequence(sequence)
			, bHasRet(false)
		{

		}

		~Responder()
		{
			if (!bHasRet)
			{
				bHasRet = true;
				dgSendRespond(uSID, uSequence, false, S());
			}
		}

		void operator()(bool bReturn, const S& spT)
		{
			if (!bHasRet)
			{
				bHasRet = true;
				dgSendRespond(uSID, uSequence, bReturn, spT);
			}
		}
	};

	template<>
	class Responder<void>
	{
	public:
		typedef fastdelegate::FastDelegate3<unsigned int, unsigned int,
			bool> SendRespondDelegate;

		SendRespondDelegate dgSendRespond;
		unsigned int uSID;
		unsigned int uSequence;
		bool bHasRet;

		Responder(const SendRespondDelegate& send, unsigned int sid, unsigned int sequence)
			: dgSendRespond(send)
			, uSID(sid)
			, uSequence(sequence)
			, bHasRet(false)
		{

		}

		~Responder()
		{
			if (!bHasRet)
			{
				dgSendRespond(uSID, uSequence, false);
			}
		}

		void operator()(bool bReturn)
		{
			bHasRet = true;
			dgSendRespond(uSID, uSequence, bReturn);
		}

	};

// 	__declspec(selectany)
// 	const char* const Responder<void>::spTName = InitRPName<void>();

}

#endif // RMRESPONDER_H__