#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <QWidget>
#include "ui_testserver.h"
#include "rpcservice\rpcresponder.hpp"

class CustomDataR;
class CustomDataT;

class TestServer : public QWidget
{
	Q_OBJECT

public:
	TestServer(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~TestServer();

protected:
	void Log(const QString& strLog);

	void ConnectState(unsigned int uSID, bool bSuccess);

	void PosRequest(unsigned int uSID, MSRPC::Responder<QPoint>& ret);
	void SetPosRequest(unsigned int uSID, const QPoint& pos, MSRPC::Responder<void>& ret);
	void TestRequest(unsigned int uSID, const CustomDataT& pos, MSRPC::Responder<CustomDataR>& ret);
	void TestRequest2(unsigned int uSID, MSRPC::Responder<void>& ret);

protected slots:
	void s_ReceiveMsg(unsigned int uConnectId, const QString& strText);
	void on_btnDisconnect_clicked();
	void on_btnReport_clicked();

private:
	Ui::TestServerClass ui;
};

#endif // TESTSERVER_H
