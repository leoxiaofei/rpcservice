#ifndef __KBSRM_H__
#define __KBSRM_H__


#include "mstools/singletemplate.hpp"
#include "rpcservice/rmmanager.h"
#include "rpcservice/lcmanager.h"
#include "rpcservice/rpctplrequest.hpp"
#include "rpcservice/rpctplreport.hpp"

// #include "rpcservice/json/msmwjsondata.hpp"
// #include "rapidjson/msjson/msjsonserialize_qt.hpp"
#include "rpcservice/xml/msmwxmldata.hpp"
#include "msserialize/xml/msxmlserialize_qt.hpp"


typedef SingleTemplate<MSRPC::RmManager> KBSRM;

class A
{
public:
	unsigned int a;
};

class CustomDataT
{
public:
	int a;
	QString b;
	A c;
};

class CustomDataR
{
public:
	double c;
	bool d;
};

namespace MSRPC
{
	template <class Archive>
	void pa_serialize(Archive& ar, A& value, int uVersion)
	{
		ar.io("a", value.a);
	}

	template <class Archive>
	void pa_serialize(Archive& ar, CustomDataT& value, int uVersion)
	{
		ar.io("a", value.a);
		ar.io("b", value.b);
		ar.io("c", value.c);
	}

	template <class Archive>
	void pa_serialize(Archive& ar, CustomDataR& value, int uVersion)
	{
		ar.io("c", value.c);
		ar.io("d", value.d);
	}
}

namespace KB
{
	extern const char szReqPos[];
	extern const char szReqSetPos[];
	extern const char szReqTest[];
	extern const char szReqTest2[];

	typedef MSRPC::RpcTplRequest<KB::szReqPos, void, QPoint> KbReqPos;
	typedef MSRPC::RpcTplRequest<KB::szReqSetPos, QPoint, void> KbReqSetPos;
	typedef MSRPC::RpcTplRequest<KB::szReqTest, CustomDataT, CustomDataR> KbReqTest;
	typedef MSRPC::RpcTplRequest<KB::szReqTest2, void, void> KbReqTest2;


// 	extern const char szRepReport[];
// 	typedef MSRPC::RpcTplReport<KB::szRepReport, QString> KbRepReport;


	void InitKBSRM();
}


#endif // __KBSRM_H__
