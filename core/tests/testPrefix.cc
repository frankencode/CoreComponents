#include <cc/Prefix>
#include <cc/String>
#include <cc/Set>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "FindKeywords",
        []{
            Set<String> keywords { "class", "public", "protected", "private", "final" };
            for (const String &key: keywords) CC_INSPECT(key);
            String text = "class Circle final: public Shape {};";
            for (long i = 0; i < text.count(); ++i)
            {
                Prefix prefix{text, i};
                Locator target;
                bool found = keywords.find(prefix, &target);
                if (found) {
                    String keyword = keywords.at(target);
                    fout() << i << ": " << keyword << nl;
                    if (i == 0) CC_CHECK(keyword == "class")
                    else if (i == 13) CC_CHECK(keyword == "final")
                    else if (i == 20) CC_CHECK(keyword == "public")
                }
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
