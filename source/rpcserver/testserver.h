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

	void PosRequest(unsigned int uSID, qint64 nTime, MSRPC::Responder<QPoint>& ret);
	void SetPosRequest(unsigned int uSID, qint64 nTime, const QPoint& pos, MSRPC::Responder<void>& ret);
	void TestRequest(unsigned int uSID, qint64 nTime, const CustomDataT& pos, MSRPC::Responder<CustomDataR>& ret);

protected slots:
	void s_ReceiveMsg(unsigned int uConnectId, const QString& strText);


private:
	Ui::TestServerClass ui;
};

#endif // TESTSERVER_H
