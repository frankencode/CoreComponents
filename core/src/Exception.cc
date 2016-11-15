#include <cc/Exception>

namespace cc {

const char* Exception::what() const throw()
{
    thread_local static String m = message();
    return m;
}

} // namespace cc
