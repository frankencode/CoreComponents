#include <cc/testing>
#include <cc/Uri>

using namespace cc;

int main(int argc, char *argv[])
{
    TestCase {
        "BreakDown",
        []{
            String text = "http://j%c3%bcrgen@example.com:8000/~j%c3%bcrgen/123.php?say=hello#part1";
            Uri uri{text};

            CC_INSPECT(text);
            CC_INSPECT(uri.toString());
            CC_INSPECT(uri.encoded());
            CC_INSPECT(uri.query());
            CC_INSPECT(uri.scheme());
            CC_INSPECT(uri.userInfo());
            CC_INSPECT(uri.host());
            CC_INSPECT(uri.port());
            CC_INSPECT(uri.path());
            CC_INSPECT(uri.fragment());

            CC_VERIFY(uri.scheme() == "http");
            CC_VERIFY(uri.userInfo() == "jürgen");
            CC_VERIFY(uri.host() == "example.com");
            CC_VERIFY(!uri.hostIsNumeric());
            CC_VERIFY(uri.port() == 8000);
            CC_VERIFY(uri.path() == "/~jürgen/123.php");
            CC_VERIFY(uri.query() == "say=hello");
            CC_VERIFY(uri.fragment() == "part1");
        }
    };

    TestCase {
        "InetAddress4",
        []{
            Uri uri{"127.0.0.1:80"};
            CC_INSPECT(uri.host());
            CC_INSPECT(uri.port());
            CC_VERIFY(uri.host() == "127.0.0.1");
            CC_VERIFY(uri.hostIsNumeric());
            CC_VERIFY(uri.port() == 80);
        }
    };

    TestCase {
        "InetAddress6",
        []{
            Uri uri{"[::1]:80"};
            CC_INSPECT(uri.host());
            CC_INSPECT(uri.port());
            CC_VERIFY(uri.host() == "::1");
            CC_VERIFY(uri.hostIsNumeric());
            CC_VERIFY(uri.port() == 80);
        }
    };

    return TestSuite{argc, argv}.run();
}
