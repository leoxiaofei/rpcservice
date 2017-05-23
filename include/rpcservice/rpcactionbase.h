#ifndef RMACTIONBASE_H__
#define RMACTIONBASE_H__

#include "rpcservice_global.h"
#include "mstools/FastDelegate.h"
#include <QObject>

namespace MSRPC
{

#define ActionName(x) public: static const char* AName(){ return #x; }\
	virtual const char* GetActionName() const { return AName(); }

	class RPCSERVICE_EXPORT RpcActionBase :public QObject
	{
	public:
		virtual ~RpcActionBase() {};
		virtual const char* GetActionName() const = 0;


	};

}

#endif // RMACTIONBASE_H__
