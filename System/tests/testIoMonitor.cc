#include <cc/IoMonitor>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Monitoring",
        []{
            const char *greetingA = "Hello, B!";
            const char *greetingB = "Hello, A!";

            IoStream a, b;
            IoStream::pair(&a, &b);
            a.write(greetingA);
            b.write(greetingB);
            a.close();
            b.close();

            IoMonitor monitor;
            monitor.watch(a, IoEvent::ReadyRead);
            monitor.watch(b, IoEvent::ReadyRead);
            int n = 0;
            monitor.wait([&](const IoActivity &){ ++n; });
            CC_CHECK_EQUALS(n, 2);

            String messageA = a.readAll();
            String messageB = b.readAll();

            CC_CHECK_EQUALS(messageB, greetingA);
            CC_CHECK_EQUALS(messageA, greetingB);
        }
    };

    return TestSuite{argc, argv}.run();
}
