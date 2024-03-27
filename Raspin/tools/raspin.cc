#include <cc/Arguments>
#include <cc/Raspin>
#include <cc/exceptions>
#include <cc/stdio>

int main(int argc, char *argv[])
{
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

        Raspin gpio;

        if (options("setup").to<String>() != "")
        {
            String setup = options("setup").to<String>();
            Raspin::Mode mode = Raspin::Mode::Input;
            if (setup == "I" || setup == "input") mode = Raspin::Mode::Input;
            else if (setup == "O" || setup == "output") mode = Raspin::Mode::Output;
            else {
                ferr("Invalid invocation syntax, see \"%% -help\"\n") << toolName;
                return 1;
            }
            Raspin::Group group = gpio.select({});
            for (int index: pins) {
                group.add(index);
            }
            group.setup(mode);
        }

        if (options("level").to<String>() == "?")
        {
            for (int index: pins) {
                Raspin::Pin pin = gpio[index];
                Raspin::Level level = pin;
                Raspin::Mode mode = pin.mode();
                const char *modeName[] = { "I", "O", "A1", "A2", "A3", "A4", "A5", "A7" };
                fout() << modeName[static_cast<int>(mode)] << "(" << dec(index, 2) << "): " << (level == Raspin::Level::Low ? "L" : "H") << nl;
            }
        }
        else if (options("level").to<String>() != "")
        {
            Raspin::Level level = options("level").to<String>() == "H" ? Raspin::Level::High : Raspin::Level::Low;
            Raspin::Group group = gpio.select({});
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
