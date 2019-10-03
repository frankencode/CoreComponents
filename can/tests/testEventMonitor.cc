#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/stdio>
#include <cc/System>
#include <cc/Thread>
#include <cc/can/VirtualCanBus>
#include <cc/can/CanLogger>
#include <cc/can/PeriodicEmitter>
#include <cc/can/EventMonitor>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class SimpleEventProcessing: public TestCase
{
    void run()
    {
        auto bus = VirtualCanBus::create();

        auto monitor = EventMonitor::create(bus->connect());
        double t0 = System::now();
        EventHandler::create(monitor, 0x181, 0.1,
            [=](const String &data){
                fout() << fixed(System::now() - t0, 3) << " -- " << data->toHex()->breakUp(2)->join(".") << nl;
            },
            [=](){
                fout() << fixed(System::now() - t0, 3) << " -- IDLE" << nl;
            }
        );
        monitor->start();

        auto emitter = PeriodicEmitter::create(bus->connect());
        Emission::create(emitter, 0x181, 0.19, "ABCD");
        emitter->start();

        Thread::sleep(1);
        bus->shutdown();
    }
};

class ComplexEventProcessing: public TestCase
{
    void run()
    {
        auto bus = VirtualCanBus::create();

        auto monitor = EventMonitor::create(bus->connect());
        double t0 = System::now();
        EventHandler::create(monitor, 0x181, 0.1,
            [=](const String &data){
                fout() << fixed(System::now() - t0, 4) << " -- (181) " << data->toHex()->breakUp(2)->join(".") << nl;
            },
            [=](){
                fout() << fixed(System::now() - t0, 4) << " -- (181) IDLE" << nl;
            }
        );
        EventHandler::create(monitor, 0x191, 0.1,
            [=](const String &data){
                fout() << fixed(System::now() - t0, 4) << " -- (191) " << data->toHex()->breakUp(2)->join(".") << nl;
            },
            [=](){
                fout() << fixed(System::now() - t0, 4) << " -- (191) IDLE" << nl;
            }
        );
        monitor->start();

        auto emitter = PeriodicEmitter::create(bus->connect());
        Emission::create(emitter, 0x181, 0.19, "ABCD");
        Emission::create(emitter, 0x191, 0.09, "01234567");
        Emission::create(emitter, 0x183, 0.1, "XxYyZz");
        emitter->start();

        Thread::sleep(1);
        bus->shutdown();
   }
};

class SimpleSafetyEventProcessing: public TestCase
{
    void run()
    {
        auto bus = VirtualCanBus::create();

        auto monitor = EventMonitor::create(bus->connect());
        double t0 = System::now();
        SafetyEventHandler::create(monitor, 0x101, 0x102, 0.09, 0.05,
            [=](const String &data) {
                fout() << fixed(System::now() - t0, 4) << " -- (101) " << data->toHex()->breakUp(2)->join(".") << nl;
            },
            [=](SafetyError error) {
                fout() << fixed(System::now() - t0, 4) << " -- (101) " << error << nl;
            }
        );
        monitor->start();

        auto emitter = PeriodicEmitter::create(bus->connect());
        SafetyEmission::create(emitter, 0x101, 0x102, 0.1, "01234567");
        emitter->start();

        Thread::sleep(1);
        bus->shutdown();
    }
};

class ComplexSafetyEventProcessing: public TestCase
{
    void run()
    {
        auto bus = VirtualCanBus::create();

        auto monitor = EventMonitor::create(bus->connect());
        double t0 = System::now();
        SafetyEventHandler::create(monitor, 0x101, 0x102, 0.1, 0.05,
            [=](const String &data) {
                fout() << fixed(System::now() - t0, 4) << " -- (101) " << data->toHex()->breakUp(2)->join(".") << nl;
            },
            [=](SafetyError error) {
                fout() << fixed(System::now() - t0, 4) << " -- (101) " << error << nl;
            }
        );
        SafetyEventHandler::create(monitor, 0x103, 0x104, 0.2, 0.1,
            [=](const String &data) {
                fout() << fixed(System::now() - t0, 4) << " -- (103) " << data->toHex()->breakUp(2)->join(".") << nl;
            },
            [=](SafetyError error) {
                fout() << fixed(System::now() - t0, 4) << " -- (104) " << error << nl;
            }
        );
        monitor->start();

        auto emitter = PeriodicEmitter::create(bus->connect());
        SafetyEmission::create(emitter, 0x101, 0x102, 0.09, "01234567");
        SafetyEmission::create(emitter, 0x103, 0x104, 0.19, "ABCD");
        emitter->start();

        Thread::sleep(1);
        bus->shutdown();
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleEventProcessing);
    CC_TESTSUITE_ADD(ComplexEventProcessing);
    CC_TESTSUITE_ADD(SimpleSafetyEventProcessing);
    CC_TESTSUITE_ADD(ComplexSafetyEventProcessing);

    return TestSuite::instance()->run(argc, argv);
}
