#include <cc/ThreadLocalSingleton>
#include "ErrorLog.h"

namespace ccnode {

ErrorLog *errorLog() { return ThreadLocalSingleton<ErrorLog>::instance(); }

} // namespace ccnode
