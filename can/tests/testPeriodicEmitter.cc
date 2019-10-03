#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/Worker>
#include <cc/Thread>
#include <cc/can/VirtualCanBus>
#include <cc/can/PeriodicEmitter>
#include <cc/can/CanLogger>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class SimpleEmissionTest: public TestCase
{
    void run() override
    {
        auto bus = VirtualCanBus::create();

        auto logger = CanLogger::create(bus->connect());

        auto emitter = PeriodicEmitter::create(bus->connect());
        Emission::create(emitter, 0x181, 0.1, "ABCD");
        Emission::create(emitter, 0x182, 0.2, "01234567");

        emitter->start();

        Thread::sleep(1);
        bus->shutdown();
    }
};

class SafetyEmissionTest: public TestCase
{
    void run() override
    {
        auto bus = VirtualCanBus::create();

        auto logger = CanLogger::create(bus->connect());

        auto emitter = PeriodicEmitter::create(bus->connect());
        SafetyEmission::create(emitter, 0x101, 0x102, 0.1, "XxYyZz");
        SafetyEmission::create(emitter, 0x103, 0x104, 0.2, "01234567");

        emitter->start();

        Thread::sleep(1);
        bus->shutdown();
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleEmissionTest);
    CC_TESTSUITE_ADD(SafetyEmissionTest);

    return TestSuite::instance()->run(argc, argv);
}
