#ifndef __RMTPLREQSYNC_H__
#define __RMTPLREQSYNC_H__

#include "mstools\rtcodeloop.h"

namespace MSRPC
{
	template<class TPL>
	class RpcTplReqSyncData
	{
	public:
		typedef typename TPL::Answer Answer;

		RpcTplReqSyncData(TPL* tpl)
			: m_tpl(tpl)
		{
			m_tpl->ConnectRespondReceiver(fastdelegate::MakeDelegate(this, 
				&RpcTplReqSyncData<TPL>::RespondReceiver));
		}

		~RpcTplReqSyncData()
		{
			m_tpl->DisconnectRespondReceiver(fastdelegate::MakeDelegate(this, 
				&RpcTplReqSyncData<TPL>::RespondReceiver));
		}

		bool WaitForRespond()
		{
			QTimer::singleShot(5000, &m_loop, SLOT(quit()));
			return m_loop.exec(QEventLoop::ExcludeUserInputEvents) == 1 && m_bReturn;
		}

		void RespondReceiver(unsigned int uSID, unsigned int uSequence, bool bReturn, const Answer& data)
		{
			m_data = data;
			m_bReturn = bReturn;
			m_loop.slot_Finish();
		}

		const Answer& GetData() const
		{
			return m_data;
		}

	private:
		RtCodeLoop m_loop;
		Answer m_data;
		TPL* m_tpl;
		bool m_bReturn;
	};

}

#endif // __RMTPLREQSYNC_H__
