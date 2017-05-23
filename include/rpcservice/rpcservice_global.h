#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RPCSERVICE_LIB)
#  define RPCSERVICE_EXPORT Q_DECL_EXPORT
# else
#  define RPCSERVICE_EXPORT Q_DECL_IMPORT
# endif
#else
# define RPCSERVICE_EXPORT
#endif
