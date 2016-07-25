#include <cc/ThreadLocalSingleton>
#include "AccessLog.h"

namespace ccnode {

AccessLog *accessLog() { return ThreadLocalSingleton<AccessLog>::instance(); }

} // namespace ccnode
