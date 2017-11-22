#include "testserver.h"
#include "rpcservice\rpcactlink.h"
#include "../common/kbsrm.h"

#include <QDateTime>
#include "rpcservice\rpcservice.h"
#include "rpcservice\xml\rpcxmlrequest.h"

using namespace KB;

typedef MSRPC::RpcXmlRequest RpcRequest;

TestServer::TestServer(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags)
{
	RpcService a;

	ui.setupUi(this);

	KB::InitKBSRM();

	MSRPC::RpcActLink* pRpcActLink =
		KBSRM::Instance().GetRpcAction<MSRPC::RpcActLink>();

	if (!pRpcActLink->Listen("0.0.0.0", 3240))
	{
		Log("Listen 3240 Failed!");
	}
	else
	{
		Log("Listen 3240 Success!");
	}

	pRpcActLink->SetStateDelegate(fastdelegate::MakeDelegate(this, &TestServer::ConnectState));


	RpcRequest* jr = KBSRM::Instance().GetDistributor<RpcRequest>();

	///连接回调
// 	if (KbReqPos* reqPos = jr->GetRequest<KbReqPos>())
// 	{
// 		reqPos->ConnectRequestReceiver(
// 			fastdelegate::MakeDelegate(this, &TestServer::PosRequest));
// 	}
// 
// 	if (KbReqSetPos* reqPos = jr->GetRequest<KbReqSetPos>())
// 	{
// 		reqPos->ConnectRequestReceiver(
// 			fastdelegate::MakeDelegate(this, &TestServer::SetPosRequest));
// 	}

	if (KbReqTest* reqTest = jr->GetRequest<KbReqTest>())
	{
		reqTest->ConnectRequestReceiver(
			fastdelegate::MakeDelegate(this, &TestServer::TestRequest));
	}

	if (KbReqTest2* reqTest2 = jr->GetRequest<KbReqTest2>())
	{
		reqTest2->ConnectRequestReceiver(
			fastdelegate::MakeDelegate(this, &TestServer::TestRequest2));
	}
}

TestServer::~TestServer()
{

}

void TestServer::Log( const QString& strLog )
{
	ui.edtMsg->appendPlainText("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	ui.edtMsg->appendPlainText(QDateTime::currentDateTime().toString());
	ui.edtMsg->appendPlainText(strLog);
	ui.edtMsg->appendPlainText("\r\n");
}

void TestServer::ConnectState(unsigned int uSID, bool bSuccess)
{
	QString strUserId = QString::number(uSID, 16).toUpper();
	if (bSuccess)
	{
		Log(QString("Client %1 Connected!").arg(uSID));

		ui.lvUser->addItem(strUserId);
	}
	else
	{
		Log(QString("Client %1 Disconnected!").arg(uSID));

		foreach(QListWidgetItem *aItem, ui.lvUser->findItems(strUserId, Qt::MatchExactly))
		{
			delete aItem;
		}
	}
}

///客户端请求Pos
void TestServer::PosRequest(unsigned int uSID, MSRPC::Responder<QPoint>& ret)
{
	///返回QPoint
	Log(QString::fromLocal8Bit("客户端请求获取Pos"));
	ret(true, QPoint(1, 2));
}

void TestServer::SetPosRequest(unsigned int uSID, const QPoint& pos, MSRPC::Responder<void>& ret)
{
	Log(QString::fromLocal8Bit("客户端请求设置Pos:") + QString("%1:%2").arg(pos.x()).arg(pos.y()));
	ret(true);
}

void TestServer::TestRequest(unsigned int uSID, 
	const CustomDataT& pos, MSRPC::Responder<CustomDataR>& ret)
{
	Log(QString::fromLocal8Bit("客户端测试请求: CustomDataT:") + QString("a:%1;b:%2").arg(pos.a).arg(pos.b));

	///客户端想要得到CustomDataR;
	CustomDataR r;
	r.c = 3.414;
	r.d = true;
	ret(true, r);
}

void TestServer::TestRequest2(unsigned int uSID, MSRPC::Responder<void>& ret)
{
	ret(true);
}

void TestServer::s_ReceiveMsg( unsigned int uConnectId, const QString& strText )
{
	Log(QString("Client %1 Say: %2").arg(uConnectId).arg(strText));

	QByteArray buffer = strText.toUtf8();
	std::reverse(buffer.begin(), buffer.end());
	//lcService::Instance().SendMsg(uConnectId, QString(buffer));
}

void TestServer::on_btnDisconnect_clicked()
{
	if (MSRPC::RpcActLink* pRpcActLink =
		KBSRM::Instance().GetRpcAction<MSRPC::RpcActLink>())
	{
		foreach(QListWidgetItem *item, ui.lvUser->selectedItems())
		{
			unsigned int uSID = item->text().toUInt(0, 16);

			pRpcActLink->Disconnect(uSID);
		}
	}
}

void TestServer::on_btnReport_clicked()
{
// 	MSRPC::RpcJsonReport* jrr = KBSRM::Instance().GetDistributor<MSRPC::RpcJsonReport>();
// 
// 	if (KbRepReport* rep = jrr->GetReport<KbRepReport>())
// 	{
// 		foreach(QListWidgetItem *item, ui.lvUser->selectedItems())
// 		{
// 			unsigned int uSID = item->text().toUInt(0, 16);
// 
// 			rep->SendReport(uSID, QString::fromLocal8Bit("这是服务主动推送报告测试。"));
// 		}
// 	}


}
