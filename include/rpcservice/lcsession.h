#ifndef __LCSESSION_H__
#define __LCSESSION_H__

#include "rpcservice_global.h"
#include "mstools/FastDelegate.h"
#include <QObject>

class QLocalSocket;


namespace MSRPC
{
	typedef fastdelegate::FastDelegate3<unsigned int, QByteArray&, quint8> ReceiveDataDelegate;

	class RPCSERVICE_EXPORT LcSession : public QObject
	{
		Q_OBJECT
	public:
		LcSession(unsigned int uSID, QLocalSocket* pSokcet, QObject* parent = 0);
		~LcSession();

		void SetReceiveDataDelegate(const ReceiveDataDelegate& dgReceiveData);

		unsigned int GetSID() const;

		void CloseSession();

		void SendData(const QByteArray& baData, quint8 eType);

	protected slots:
		void slot_ReadyRead();
		void slot_Disconnect();

	signals:
		void signal_SocketError();

	private:
		class Impl;
		QScopedPointer<Impl> m_pImpl;
	};

}


#endif // __LCSESSION_H__
