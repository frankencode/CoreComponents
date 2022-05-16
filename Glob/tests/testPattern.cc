#include <cc/Pattern>
#include <cc/testing>
#include <cc/DEBUG>

using namespace cc;
using namespace cc;

int main(int argc, char **argv)
{
    TestCase {
        "Globbing",
        []{
            Pattern pattern { "*.(c|h){..2:[^.]}^" };

            CC_INSPECT(pattern.explain());

            CC_CHECK(pattern.match("/home/hans/glück.hh"));
            CC_CHECK(pattern.match("a.b.c"));
            CC_CHECK(!pattern.match("a.b.c.d.e.f"));
            CC_CHECK(pattern.match("test.cpp"));
        }
    };

    TestCase {
        "UriDispath",
        []{
            List<String> services {
                "glibc.*",
                "cckit.*",
                "books.*",
                "test.*",
                "*httpecho*"
            };

            for (const String &service: services)
                fout() << service << ": " << Pattern{service}.match("cckit.cyblogic.com") << nl;
        }
    };

    TestCase {
        "CxxPathGlobbing",
        []{
            Pattern pattern { "*/include/*" };
            CC_CHECK(pattern.match("/usr/include/c++/5/list"));
            CC_CHECK(!pattern.match("/usr/lib/libnegative.so"));
        }
    };

    TestCase {
        "MailAddress",
        []{
            Pattern pattern {
                "(^>[.-])"
                "{1..:(^>..|--)([a..z]|[A..Z]|[0..9]|[.-])}"
                "(^<[.-])"
                "@"
                "(^>[.-])"
                "{1..:(^>..|--)([a..z]|[A..Z]|[0..9]|[.-])}"
                "(^<[.-])"
            };

            CC_INSPECT(pattern.explain());

            CC_CHECK(pattern.match("info@cyblogic.com"));
            CC_CHECK(!pattern.match("@lala.de"));
            CC_CHECK(!pattern.match("otto@übertragungsfehler.de"));
            CC_CHECK(!pattern.match(""));
            CC_CHECK(pattern.match("a@B"));
            CC_CHECK(!pattern.match("ia--b@C"));
            CC_CHECK(!pattern.match("a-b.@C"));
            CC_CHECK(!pattern.match("b-@C"));
            CC_CHECK(pattern.match("a.b.c@d.e.f"));
        }
    };

    return TestSuite{argc, argv}.run();
}
