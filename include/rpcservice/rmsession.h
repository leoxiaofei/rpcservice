#ifndef __RMSESSION_H__
#define __RMSESSION_H__

#include "rpcservice_global.h"
#include "mstools/FastDelegate.h"
#include <QObject>

class QTcpSocket;


namespace MSRPC
{
	typedef fastdelegate::FastDelegate3<unsigned int, QByteArray&, quint8> ReceiveDataDelegate;

	class RPCSERVICE_EXPORT RmSession : public QObject
	{
		Q_OBJECT
	public:
		RmSession(unsigned int uSID, QTcpSocket* pSokcet, QObject* parent = 0);
		~RmSession();

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
#endif // __RMSESSION_H__
