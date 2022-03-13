#include <cc/String>
#include <cc/Casefree>
#include <cc/Unicode>
#include <cc/Utf16>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "ConstructionConcatenation",
        []{
            String a = "123";
            String b { "abc" };
            String c = a + b;
            String d = b.paste(1, 2, "...");
            a[0] = 'x';
            b = a;
            CC_INSPECT(a);
            CC_INSPECT(b);
            CC_INSPECT(c);
            CC_INSPECT(d);
            CC_CHECK(a == "x23");
            CC_CHECK(b == "x23");
            CC_CHECK(c == "123abc");
            CC_CHECK(d == "a...c");
        }
    };

    TestCase {
        "CasefreeComparisms",
        []{
            CC_CHECK(Casefree<String>{"Hello"} == Casefree<String>{"hello"});
            CC_CHECK(Casefree<String>{"abra"} < Casefree<String>{"Kadabra"});
            CC_CHECK(Casefree<String>{"abra"} < Casefree<String>{"ABRAKADABRA"});
            CC_CHECK(Casefree<String>{"abra"} != Casefree<String>{"abrakadabra"});
        }
    };

    TestCase {
        "UnicodeIteration",
        []{
            String s = "Привет!";
            for (auto ch: Unicode{s}) CC_INSPECT(ch);
        }
    };

    TestCase {
        "UnicodeEscapes",
        []{
            String s = "\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!";
            CC_CHECK_EQUALS(s.expanded(), "Привет!");
        }
    };

    return TestSuite{argc, argv}.run();
}
