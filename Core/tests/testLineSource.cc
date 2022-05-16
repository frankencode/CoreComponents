#include <cc/LineSource>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "TextIteration",
        []{
            String text =
                "Hello, world!\n"
                "abrakadabra\n"
                "\n"
                "123";

            List<String> lines;
            for (auto line: LineSource{text})
                lines << line;

            CC_CHECK_EQUALS(lines[0], "Hello, world!");
            CC_CHECK_EQUALS(lines[1], "abrakadabra");
            CC_CHECK_EQUALS(lines[2], "");
            CC_CHECK_EQUALS(lines[3], "123");
            CC_CHECK(lines.count() == 4);
        }
    };

    return TestSuite{argc, argv}.run();
}
