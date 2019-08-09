#include <cc/ThreadLocalSingleton>
#include "AccessLog.h"

namespace ccnode {

AccessLog *AccessLog::instance()
{
    return ThreadLocalSingleton<AccessLog>::instance();
}

} // namespace ccnode
