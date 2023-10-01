#include <cc/Arguments>
#include <cc/rpi/Gpio>
#include <cc/exceptions>
#include <cc/stdio>

namespace cc::rpi {

} // namespace  cc::rpi

int main(int argc, char *argv[])
{
    using namespace cc::rpi;
    using namespace cc;

    String toolName = String{argv[0]}.fileName();

    try {
        Map<String, Variant> options;
        options.insert("setup", "");
        options.insert("level", "");

        Arguments arguments{argc, argv};
        List<String> items = arguments.read(&options);

        if (items.count() == 0) {
            ferr("Invalid invocation syntax, see \"%% -help\"\n") << toolName;
            return 1;
        }

        List<int> pins;
        for (const String &item: items) {
            bool ok = true;
            pins << item.toInt(&ok);
            if (!ok) {
                ferr("Invalid invocation syntax, see \"%% -help\"\n") << toolName;
                return 1;
            }
        }

        Gpio gpio;

        if (options("setup").to<String>() != "")
        {
            String setup = options("setup").to<String>();
            Gpio::Mode mode = Gpio::Mode::Input;
            if (setup == "I" || setup == "input") mode = Gpio::Mode::Input;
            else if (setup == "O" || setup == "output") mode = Gpio::Mode::Output;
            else {
                ferr("Invalid invocation syntax, see \"%% -help\"\n") << toolName;
                return 1;
            }
            Gpio::Group group = gpio.select({});
            for (int index: pins) {
                group.add(index);
            }
            group.setup(mode);
        }

        if (options("level").to<String>() == "?")
        {
            for (int index: pins) {
                Gpio::Pin pin = gpio[index];
                Gpio::Level level = pin;
                Gpio::Mode mode = pin.mode();
                const char *modeName[] = { "I", "O", "A1", "A2", "A3", "A4", "A5", "A7" };
                fout() << modeName[static_cast<int>(mode)] << "(" << dec(index, 2) << "): " << (level == Gpio::Level::Low ? "L" : "H") << nl;
            }
        }
        else if (options("level").to<String>() != "")
        {
            Gpio::Level level = options("level").to<String>() == "H" ? Gpio::Level::High : Gpio::Level::Low;
            Gpio::Group group = gpio.select({});
            for (int index: pins) {
                group.add(index);
            }
            group = level;
        }
    }
    catch (HelpRequest &)
    {
        fout(
            "Usage: %% [OPTION] [GPIO]...\n"
            "Configure, read and write Raspberry Pi's GPIO register\n"
            "\n"
            "Options:\n"
            "  -setup=[I|O]    setup function\n"
            "  -level=[?|L|H]  get or group level\n"
            "\n"
            "Examples:\n"
            "  %% -setup=O 3 4  # Setup GPIO3 and GPIO4 for output\n"
            "  %% -level=H 3 4  # Group levels of GPIO3 and GPIO4 to high\n"
            "  %% -level=? 3 4  # Get levels of GPIO3 and GPIO4\n"
        ) << toolName << toolName << toolName << toolName;
        return 1;
    }
    catch (SystemResourceError &ex) {
        ferr() << toolName << ": " << ex << nl;
    }
    #ifdef NDEBUG
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }
    #endif

    return 0;
}
