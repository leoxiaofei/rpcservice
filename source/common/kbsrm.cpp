#include "kbsrm.h"

#include "rpcservice/json/rmjsonreport.h"
#include "rpcservice/json/rpcjsonrequest.h"


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

	void InitKBSRM()
	{
		MSRPC::RpcJsonRequest* pJsonRequest = new MSRPC::RpcJsonRequest;
// 		pJsonRequest->RegRequest<KbReqShow>();
// 		pJsonRequest->RegRequest<KbReqSize>();
 		pJsonRequest->RegRequest<KbReqPos>();
// 		pJsonRequest->RegRequest<KbReqSetSize>();
 		pJsonRequest->RegRequest<KbReqSetPos>();
		pJsonRequest->RegRequest<KbReqTest>();
		pJsonRequest->RegRequest<KbReqTest2>();

		KBSRM::Instance().RegDistributor(pJsonRequest);
	}

}