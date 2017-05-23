#ifndef RTCODELOOP_H__
#define RTCODELOOP_H__

#include <QEventLoop>


class RtCodeLoop : public QEventLoop
{
	Q_OBJECT

public slots:
	void slot_Finish(int nReturnCode = 1)
	{
		exit(nReturnCode);
	}

};


#endif // RTCODELOOP_H__
