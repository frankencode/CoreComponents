#include <cc/Exception>

namespace cc {

const char* Exception::what() const throw()
{
    thread_local static string m = message();
    return m;
}

} // namespace cc
