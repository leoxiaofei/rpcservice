#include "testclient.h"
#include "rpcservice/rpcactlink.h"
#include "../common/kbsrm.h"
#include "rpcservice/json/rpcjsonrequest.h"
#include <QDateTime>
#include "rpcservice/json/rpcjsonreport.h"

using namespace KB;

TestClient::TestClient(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags)
{
	ui.setupUi(this);

	KB::InitKBSRM();

// 	connect(p, SIGNAL(sg_Connected(unsigned int)), this, SLOT(s_Connected(unsigned int)));
// 	connect(p, SIGNAL(sg_Disconnected(unsigned int)), this, SLOT(s_Disconnected(unsigned int)));
// 	connect(p, SIGNAL(sg_ReceiveMsg(unsigned int, const QString&)), 
// 		this, SLOT(s_ReceiveMsg(unsigned int, const QString&)));
	
	MSRPC::RpcJsonRequest* jr = KBSRM::Instance().GetDistributor<MSRPC::RpcJsonRequest>();

	if (KbReqPos* reqPos = jr->GetRequest<KbReqPos>())
	{
		reqPos->ConnectRespondReceiver(
			fastdelegate::MakeDelegate(this, &TestClient::PosRespond));
	}

	if (KbReqSetPos* reqPos = jr->GetRequest<KbReqSetPos>())
	{
		reqPos->ConnectRespondReceiver(
			fastdelegate::MakeDelegate(this, &TestClient::SetPosRespond));
	}

	if (KbReqTest* reqTest = jr->GetRequest<KbReqTest>())
	{
		reqTest->ConnectRespondReceiver(
			fastdelegate::MakeDelegate(this, &TestClient::TestRespond));
	}

	MSRPC::RpcActLink* pRpcActLink =
		KBSRM::Instance().GetRmAction<MSRPC::RpcActLink>();
	pRpcActLink->SetStateDelegate(fastdelegate::MakeDelegate(this, &TestClient::ConnectState));

	MSRPC::RpcJsonReport* jrr = KBSRM::Instance().GetDistributor<MSRPC::RpcJsonReport>();

	if (KbRepReport* rep = jrr->GetReport<KbRepReport>())
	{
		rep->ConnectReportReceiver(
			fastdelegate::MakeDelegate(this, &TestClient::TestReport));
	}
}

TestClient::~TestClient()
{

}

void TestClient::on_btnConnect_clicked()
{
	MSRPC::RpcActLink* pRpcActLink =
		KBSRM::Instance().GetRmAction<MSRPC::RpcActLink>();
	if (pRpcActLink->Connect("127.0.0.1", 3240))
	{
		ui.btnConnect->setEnabled(false);
	}
	//lcService::Instance().ConnectLocal(ui.spPort->value());
}

void TestClient::Log( const QString& strLog )
{
	ui.edtMsg->appendPlainText("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	ui.edtMsg->appendPlainText(QDateTime::currentDateTime().toString());
	ui.edtMsg->appendPlainText(strLog);
	ui.edtMsg->appendPlainText("\r\n");
}

void TestClient::ConnectState(unsigned int uSID, bool bSuccess)
{
	bSuccess ? m_uServerId = uSID : m_uServerId = 0;

	if (bSuccess)
	{
		Log("Connect Server Success!");
		ui.btnDisconnect->setEnabled(true);
	}
	else
	{
		Log("Disconnect!");
		ui.btnConnect->setEnabled(true);
		ui.btnDisconnect->setEnabled(false);
	}
}

void TestClient::PosRespond(unsigned int uSID, unsigned int uSequence, qint64 nTime, bool bSuccess, const QPoint& pos)
{
	Log("PosRespond:" + QString("%1:%2").arg(pos.x()).arg(pos.y()));
}

void TestClient::SetPosRespond(unsigned int uSID, unsigned int uSequence, qint64 nTime, bool bSuccess)
{
	Log("SetPosRespond.");
}

void TestClient::TestRespond(unsigned int uSID, unsigned int uSequence, 
	qint64 nTime, bool bSuccess, const CustomDataR& data)
{
	Log("TestRespond:" + QString("c:%1;d:%2").arg(data.c).arg(data.d));
}

void TestClient::TestReport(unsigned int uSID, qint64 nTime, const QString& strValue)
{
	Log("TestReport:" + strValue);
}

void TestClient::on_btnSend_clicked()
{
	QString strText = ui.edtSay->text();
	if (!strText.isEmpty())
	{
		ui.edtSay->clear();

	}
}

void TestClient::on_btnGetPos_clicked()
{
	MSRPC::RpcJsonRequest* jr = KBSRM::Instance().GetDistributor<MSRPC::RpcJsonRequest>();

	if (KbReqPos* reqPos = jr->GetRequest<KbReqPos>())
	{
		reqPos->SendRequest(m_uServerId);
	}
}

void TestClient::on_btnSetPos_clicked()
{
	MSRPC::RpcJsonRequest* jr = KBSRM::Instance().GetDistributor<MSRPC::RpcJsonRequest>();

	if (KbReqSetPos* reqPos = jr->GetRequest<KbReqSetPos>())
	{
		reqPos->SendRequest(m_uServerId, QPoint(3, 4));
	}
}

void TestClient::on_btnTest_clicked()
{
	MSRPC::RpcJsonRequest* jr = KBSRM::Instance().GetDistributor<MSRPC::RpcJsonRequest>();

	if (KbReqTest* reqTest = jr->GetRequest<KbReqTest>())
	{
		CustomDataT t;
		t.a = 1234;
		t.b = QString::fromLocal8Bit("ÖÐÎÄ²âÊÔ£¡");
		reqTest->SendRequest(m_uServerId, t);
	}
}

void TestClient::on_btnTest2_clicked()
{

}

void TestClient::on_btnDisconnect_clicked()
{
	MSRPC::RpcActLink* pRpcActLink =
		KBSRM::Instance().GetRmAction<MSRPC::RpcActLink>();
	pRpcActLink->Disconnect(m_uServerId);
}

void TestClient::s_ReceiveMsg( unsigned int uConnectId, const QString& strText )
{
	Log(QString("Server Say: %1").arg(strText));

}
