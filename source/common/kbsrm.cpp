#include "kbsrm.h"
#include "rpcservice\xml\rpcxmlrequest.h"

// #include "rpcservice/json/rpcjsonreport.h"
// #include "rpcservice/json/rpcjsonrequest.h"


namespace KB
{
	char const szReqTest[] = "Test";

	char const szReqTest2[] = "Test2";

	char const szReqShow[] = "Show";

	char const szReqPos[] = "Pos";

	char const szReqSize[] = "Size";

	char const szReqSetPos[] = "SetPos";

	char const szReqSetSize[] = "SetSize";

	char const szReqSwitch[] = "Switch";

	char const szRepReport[] = "Report";

	void InitKBSRM()
	{
// 		MSRPC::RpcJsonRequest* pJsonRequest = 
// 			KBSRM::Instance().RegDistributor<MSRPC::RpcJsonRequest>();
// // 		pJsonRequest->RegRequest<KbReqShow>();
// // 		pJsonRequest->RegRequest<KbReqSize>();
//  		pJsonRequest->RegRequest<KbReqPos>();
// // 		pJsonRequest->RegRequest<KbReqSetSize>();
//  		pJsonRequest->RegRequest<KbReqSetPos>();
// 		pJsonRequest->RegRequest<KbReqTest>();
// 		pJsonRequest->RegRequest<KbReqTest2>();

// 		MSRPC::RpcJsonReport* pJsonReport =
// 			KBSRM::Instance().RegDistributor<MSRPC::RpcJsonReport>();
// 
// 		pJsonReport->RegReport<KbRepReport>();

		MSRPC::RpcXmlRequest* pXmlRequest = 
			KBSRM::Instance().RegDistributor<MSRPC::RpcXmlRequest>();
//  		pXmlRequest->RegRequest<KbReqPos>();
//  		pXmlRequest->RegRequest<KbReqSetPos>();
		pXmlRequest->RegRequest<KbReqTest>();
		pXmlRequest->RegRequest<KbReqTest2>();

	}

}