#include <cc/ThreadLocalSingleton>
#include "ErrorLog.h"

namespace ccnode {

ErrorLog *ErrorLog::instance() { return ThreadLocalSingleton<ErrorLog>::instance(); }

} // namespace ccnode
