#include "lcsession.h" 

#include <QLocalSocket>

namespace MSRPC
{
	struct RmHeader
	{
		unsigned short uCheck : 16;
		unsigned short uReserve : 8;
		unsigned short uType : 8;
		unsigned short uTotalBag : 8;
		unsigned short uCurBag : 8;
		unsigned short uBagSize : 16;
	};

	const unsigned short g_uCheck = 0xDF;
	const unsigned short g_uMaxBagSize = 0xFFFF;

	class LcSession::Impl
	{
	public:
		Impl(unsigned int sid, QLocalSocket* so)
		: uSID(sid)
		, pSokcet(so)
		, vecMaxBagBuffer(g_uMaxBagSize + 1)
		{}

		unsigned int		uSID;
		QLocalSocket*			pSokcet;
		QVector<char>		vecMaxBagBuffer;
		QByteArray			baReceive;
		ReceiveDataDelegate	dgReceiveData;
	};

	LcSession::LcSession(unsigned int uSID, QLocalSocket* pSokcet, QObject* parent)
		: QObject(parent)
		, m_pImpl(new Impl(uSID, pSokcet))
	{
		pSokcet->setParent(this);
		QObject::connect(pSokcet, SIGNAL(readyRead()), this, SLOT(slot_ReadyRead()));
		QObject::connect(pSokcet, SIGNAL(disconnected()), this, SLOT(slot_Disconnect()));
		QObject::connect(pSokcet, SIGNAL(error(QLocalSocket::LocalSocketError)),
			this, SIGNAL(signal_SocketError()));
	}

	LcSession::~LcSession()
	{
		if (m_pImpl->pSokcet->state() != QLocalSocket::UnconnectedState)
		{
			m_pImpl->pSokcet->disconnectFromServer();
			m_pImpl->pSokcet->waitForDisconnected(1000);
		}
	}
	
	unsigned int LcSession::GetSID() const
	{
		return m_pImpl->uSID;
	}

	void LcSession::CloseSession()
	{
		deleteLater();
	}

	void LcSession::SendData(const QByteArray& baData, quint8 eType)
	{
		QLocalSocket* pSokcet = m_pImpl->pSokcet;
		int nLen = baData.size();
		int nCurPos = 0;

		RmHeader rmHeader;
		rmHeader.uCheck = g_uCheck;
		rmHeader.uReserve = 0;
		rmHeader.uType = eType;
		rmHeader.uTotalBag = nLen / g_uMaxBagSize + 1;
		rmHeader.uCurBag = 0;
		rmHeader.uBagSize = 0;

		qint64 nSend(0);

		while (nCurPos < nLen)
		{
			++rmHeader.uCurBag;
			rmHeader.uBagSize = qMin<int>(g_uMaxBagSize, nLen - nCurPos);

			nSend = pSokcet->write((const char *)(&rmHeader), sizeof(RmHeader));
			nSend = pSokcet->write(baData.data() + nCurPos, rmHeader.uBagSize);
			nCurPos += rmHeader.uBagSize;
		}
	}

	void LcSession::slot_ReadyRead()
	{
		bool bSuccess(false);
		QLocalSocket* pSokcet = m_pImpl->pSokcet;
		char* szMaxBagBuffer = m_pImpl->vecMaxBagBuffer.data();
		QByteArray& baReceive = m_pImpl->baReceive;

		do
		{
			RmHeader rmHeader;
			qint64 nHeaderSize = sizeof(RmHeader);

			//////////////////////////////////////////////////////////////////////////
			/// 读取数据头
			if (nHeaderSize != pSokcet->read((char*)(&rmHeader), nHeaderSize))
			{
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			/// 验证数据头
			if (rmHeader.uCheck != g_uCheck
				&& rmHeader.uBagSize <= g_uMaxBagSize
				&& rmHeader.uCurBag <= rmHeader.uTotalBag)
			{
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			/// 读取数据体
			int nCurPos = 0;

			do
			{
				if (pSokcet->bytesAvailable() <= 0 && !pSokcet->waitForReadyRead(6000))
				{
					break;
				}

				nCurPos += pSokcet->read(szMaxBagBuffer + nCurPos, rmHeader.uBagSize - nCurPos);

			} while (nCurPos < rmHeader.uBagSize);


			if (rmHeader.uBagSize != nCurPos)
			{
				break;
			}

			baReceive.append(szMaxBagBuffer, nCurPos);

			if (rmHeader.uTotalBag == rmHeader.uCurBag)
			{
				m_pImpl->dgReceiveData(this, baReceive, rmHeader.uType);
				baReceive.clear();
			}

			bSuccess = pSokcet->bytesAvailable() <= 0;

		} while (!bSuccess);


		if (!bSuccess)
		{
			baReceive.clear();
		}
	}

	void LcSession::slot_Disconnect()
	{
		CloseSession();
	}

	void LcSession::SetReceiveDataDelegate(const ReceiveDataDelegate& dgReceiveData)
	{
		m_pImpl->dgReceiveData = dgReceiveData;
	}

}