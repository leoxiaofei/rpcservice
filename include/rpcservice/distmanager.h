#ifndef DISTMANAGER_H__
#define DISTMANAGER_H__

#include "mstools/FastDelegate.h"

#include <QMap>

class QByteArray;

namespace MSRPC
{
	class RpcDistributor;

template<class COMMGR>
class DistManager : public COMMGR
{
public:
	DistManager()
	{
		COMMGR::SetReceiveDataDelegate(fastdelegate::MakeDelegate(
			this, &DistManager::ReceiveData));
	}

	~DistManager()
	{

	}

	void RegDistributor(RpcDistributor* pDistributor)
	{
		pDistributor->SetSendDataDelegate(fastdelegate::MakeDelegate(
			this, &COMMGR::SendData));
		m_mapDistributor[pDistributor->GetType()] = pDistributor;
	}

	RpcDistributor* GetDistributor(int nDisType) const
	{
		return m_mapDistributor.value(nDisType, 0);
	}

	template<class T>
	T* RegDistributor()
	{
		T* t = new T;
		RegDistributor(t);
		return t;
	}

	template<class T>
	T* GetDistributor() const
	{
		return static_cast<T*>(GetDistributor(T::Type));
	}

	void ReceiveData(unsigned int uSID, QByteArray& baData, quint8 eType)
	{
		QMap<quint8, RpcDistributor*>::const_iterator ciFind = m_mapDistributor.constFind(eType);
		if (ciFind != m_mapDistributor.constEnd())
		{
			//qDebug()<<baData;
			(*ciFind)->ReceiveData(uSID, baData);
		}
	}

private:
	QMap<quint8, RpcDistributor*> m_mapDistributor;
};

}

#endif // DISTMANAGER_H__
