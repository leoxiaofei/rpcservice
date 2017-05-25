#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include <QWidget>
#include "ui_testclient.h"

class CustomDataR;

class TestClient : public QWidget
{
	Q_OBJECT

public:
	TestClient(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~TestClient();

protected:
	void Log(const QString& strLog);

	void ConnectState(unsigned int uSID, bool bSuccess);

	void PosRespond(unsigned int uSID, unsigned int uSequence, bool bSuccess, const QPoint& pos);
	void SetPosRespond(unsigned int uSID, unsigned int uSequence, bool bSuccess);
	void TestRespond(unsigned int uSID, unsigned int uSequence, bool bSuccess, const CustomDataR& data);
	void TestReport(unsigned int uSID, const QString& strValue);

protected slots:
	void on_btnConnect_clicked();
	void on_btnSend_clicked();
	void on_btnGetPos_clicked();
	void on_btnSetPos_clicked();
	void on_btnTest_clicked();
	void on_btnTest2_clicked();
	void on_btnDisconnect_clicked();
	
	void s_ReceiveMsg(unsigned int uConnectId, const QString& strText);

private:
	Ui::TestClientClass ui;
	unsigned int m_uServerId;
};

#endif // TESTCLIENT_H
