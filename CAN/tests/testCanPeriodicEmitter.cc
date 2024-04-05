#include <cc/CanPeriodicEmitter>
#include <cc/VirtualCanBus>
#include <cc/CanLogger>
#include <cc/Thread>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "SimpleEmissionTest",
        []{
            VirtualCanBus bus;

            CanLogger logger { bus.connect() };

            CanPeriodicEmitter emitter { bus.connect() };
            emitter.addEmission({0x181, 0.1, String{"ABCD"}});
            emitter.addEmission({0x182, 0.2, String{"01234567"}});

            logger.start();
            emitter.start();

            Thread::sleep(1);
            bus.shutdown();
        }
    };

    return TestSuite{argc, argv}.run();
}
