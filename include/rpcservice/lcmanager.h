#ifndef __LCMANAGER_H__
#define __LCMANAGER_H__

#include "rpcservice_global.h"
#include "mstools/FastDelegate.h"
#include "distmanager.h"

#include <QObject>
#include <QMap>

class QLocalServer;
class QLocalSocket;
class QHostAddress;

namespace MSRPC
{
	class LcSession;
	class RpcDistributor;
	class RpcActionBase;

	typedef fastdelegate::FastDelegate3<unsigned int, QByteArray&, quint8> ReceiveDataDelegate;

class RPCSERVICE_EXPORT LcComMgr : public QObject
{
	Q_OBJECT
	class Impl;
public:
	LcComMgr();
	~LcComMgr();

	void Listen(const QString& strHostAddr, quint16 uPort);
	void Connect(const QString& strHostName, quint16 uPort);
	void Disconnect(unsigned int uSID);
	void CloseAll();

	Q_INVOKABLE void SendData(unsigned int uSID, const QByteArray& baData, quint8 eType);

	QLocalServer* GetServer() const;
	LcSession* GetLcSession(unsigned int uSID) const;
	QThread* GetThread() const;

	template<class T>
	T* RegRpcAction()
	{
		T* tRet = new T;
		RegRpcActionBase(tRet);
		return tRet;
	}

	template<class T>
	T* GetRpcAction() const
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
	const QMap<unsigned int, LcSession*>& GetLcSessions() const;
	bool IsIdle() const;

signals:
	void signal_ListenResult(int eError);
	void signal_ConnectResult(int eError);
	void signal_SessionStart(unsigned int uSID);
	void signal_SessionEnd(unsigned int uSID);
	void signal_SessionError(unsigned int uSID);
	void signal_ReceiveData(unsigned int uSID, const QByteArray& baData, quint8 eType);

protected slots:
	void slot_Connect(const QString& strHostName, quint16 uPort);
	void slot_Listen(const QString& strHostAddr, quint16 uPort);
	void slot_Disconnect(unsigned int uSID);
	void slot_NewConnection();
	void slot_SessionDestroyed(QObject *obj);
	void slot_SessionError();

protected:
	LcSession* CreateSession(QLocalSocket* spSocket);
	void ReceiveData(const LcSession* rSession, QByteArray& baData, quint8 eType);
	
	void InitActions();

private:
	QScopedPointer<Impl> m_pImpl;
};

typedef DistManager<LcComMgr> LcManager;

};

#endif // __RMMANAGER_H__
