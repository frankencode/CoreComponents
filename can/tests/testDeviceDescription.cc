#include <cc/testing/TestSuite>
#include <cc/File>
#include <cc/debug>
#include <cc/can/DeviceDescription>
#include <cc/can/DeviceConfiguration>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class DeciveDescriptionTest: public TestCase
{
    void run() override
    {
        string edsPath = string{__FILE__}->reducePath()->extendPath("BatteryModule.eds");
        string edsText = File::open(edsPath)->map();
        auto deviceDescription = DeviceDescription::parse(edsText);

        string edsText2 = deviceDescription->toString();
        // CC_VERIFY(edsText == edsText2);

        auto deviceDescription2 = DeviceDescription::parse(edsText2);
        // string edsPath2 = edsPath->replace(".eds", "_2.eds");
        // File::open(edsPath2, OpenFlags::WriteOnly|OpenFlags::Create)->write(deviceDescription2->toString());
        CC_VERIFY(deviceDescription->equals(deviceDescription2));
    }
};


class DeviceConfigurationTest: public TestCase
{
    void run() override
    {
        string dcfPath = string{__FILE__}->reducePath()->extendPath("BatteryModule.dcf");
        string dcfText = File::open(dcfPath)->map();
        auto deviceConfiguration = DeviceConfiguration::parse(dcfText);

        string dcfText2 = deviceConfiguration->toString();
        CC_INSPECT(dcfText2);
        // CC_VERIFY(edsText == edsText2);

        auto deviceConfiguration2 = DeviceConfiguration::parse(dcfText2);
        // string edsPath2 = edsPath->replace(".eds", "_2.eds");
        // File::open(edsPath2, OpenFlags::WriteOnly|OpenFlags::Create)->write(deviceDescription2->toString());
        CC_VERIFY(deviceConfiguration->equals(deviceConfiguration2));
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(DeciveDescriptionTest);
    CC_TESTSUITE_ADD(DeviceConfigurationTest);

    return TestSuite::instance()->run(argc, argv);
}
