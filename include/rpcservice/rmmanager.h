#ifndef __RMMANAGER_H__
#define __RMMANAGER_H__

#include "rpcservice_global.h"

#include <QObject>
#include <QMap>

class QTcpServer;
class QTcpSocket;

namespace MSRPC
{
	class RmSession;
	class RpcDistributor;
	class RpcActionBase;

class RPCSERVICE_EXPORT RmManager : public QObject
{
	Q_OBJECT
	class Impl;
public:
	RmManager();
	~RmManager();

	void Listen(const QString& strHostAddr, quint16 uPort);
	void Connect(const QString& strHostName, quint16 uPort);
	void Disconnect(unsigned int uSID);
	void CloseAll();

	Q_INVOKABLE void SendData(unsigned int uSID, const QByteArray& baData, quint8 eType);

	QTcpServer*	GetRmServer() const;
	RmSession* GetRmSession(unsigned int uSID) const;
	QThread*	GetThread() const;

	void RegDistributor(RpcDistributor* pRmDistributor);
	RpcDistributor* GetDistributor(int nRmDisType) const;

	template<class T>
	inline T* RegDistributor()
	{
		T* p = new T;
		RegDistributor(p);
		return p;
	}

	template<class T>
	inline T* GetDistributor() const
	{
		return static_cast<T*>(GetDistributor(T::Type));
	}

	template<class T>
	inline T* RegRmAction()
	{
		T* tRet = new T;
		RegRpcActionBase(tRet);
		return tRet;
	}

	template<class T>
	inline T* GetRmAction() const
	{
		return static_cast<T*>(
			GetRpcActionBase(T::AName()));
	}

	void RegRpcActionBase(RpcActionBase* pRmAction);
	RpcActionBase* GetRpcActionBase(const char* RmActionName) const;

	///当会话出现错误时，是否自动销毁。
	void SetAutoDestroyed(bool bAutoDestroy);
	bool GetAutoDestroyed() const;

	///获取当前全部会话
	const QMap<unsigned int, RmSession*>& GetRmSessions() const;
	bool IsIdle() const;

signals:
	void signal_ListenResult(int eError);
	void signal_ConnectResult(int eError);
	void signal_SessionStart(unsigned int uSID);
	void signal_SessionEnd(unsigned int uSID);
	void signal_SessionError(unsigned int uSID);
	void signal_ReceiveData(unsigned int uSID, const QByteArray& baData, quint8 eType);

protected slots:
	void slot_Listen(const QString& strHostAddr, quint16 uPort);
	void slot_Connect(const QString& strHostName, quint16 uPort);
	void slot_Disconnect(unsigned int uSID);
	void slot_NewConnection();
	void slot_SessionDestroyed(QObject *obj);
	void slot_SessionError();

protected:
	RmSession* CreateSession(QTcpSocket* spSocket);
	void ReceiveData(const RmSession* rSession, QByteArray& baData, quint8 eType);
	
	void InitActions();

private:
	QScopedPointer<Impl> m_pImpl;
};

};

#endif // __RMMANAGER_H__
