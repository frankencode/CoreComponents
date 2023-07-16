#include <cc/IoStream>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Pairing",
        []{
            const char *greeting = "Hello, B!";
            IoStream a, b;
            IoStream::pair(&a, &b);
            a.write(greeting);
            a.close();
            String message = b.readAll();
            CC_CHECK_EQUALS(message, greeting);
        }
    };

    return TestSuite{argc, argv}.run();
}
