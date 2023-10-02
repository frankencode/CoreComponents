#include <cc/rpi/Gpio>
#include <cc/Thread>
#include <cc/SignalMaster>
#include <cc/Arguments>
#include <cc/System>
#include <cc/Timezone>
#include <cc/stdio>

namespace cc::rpi {

const uint8_t number[] {
    0b11101011, // "0"
    0b00101000, // "1"
    0b10110011, // "2"
    0b10111010, // "3"
    0b01111000, // "4"
    0b11011010, // "5"
    0b11011011, // "6"
    0b10101000, // "7"
    0b11111011, // "8"
    0b11111010, // "9"
};

const uint8_t letter[] {
    0b11111001, // "A"
    0b01011011, // "b"
    0b11000011, // "C"
    0b00111011, // "d"
    0b11010011, // "E"
    0b11010001, // "F"
    0b11001011, // "G"
    0b01111001, // "H"
    0b00101000, // "I"
    0b00101010, // "J"
    0b11011001, // "K"
    0b01000011, // "L"
    0b10111010, // "M"
    0b11101001, // "N"
    0b11101011, // "O"
    0b11110001, // "P"
    0b11111000, // "q"
    0b00010001, // "r"
    0b11011010, // "S"
    0b00111000, // "T"
    0b01101011, // "U"
    0b01101011, // "V"
    0b10111010, // "W"
    0b01101001, // "X"
    0b01111000, // "Y"
    0b10110011, // "Z"
};

} // namespace cc::rpi

int main(int argc, char *argv[])
{
    using namespace cc::rpi;
    using namespace cc;

    List<int> bcmAssign { 17, 27, 22, 5, 6, 12, 13, 16, 20, 19, 26, 21 };

    Channel<Signal> signalsReceived;

    Thread::blockSignals(SignalSet::full());
    SignalMaster signalMaster { signalsReceived };
    signalMaster.start();

    List<int> anAssign { 1, 2, 3, 4, 5, 7, 10, 11 };
    List<int> catAssign { 6, 8, 9, 12 };

    for (int &index: anAssign) {
        index = bcmAssign[index - 1];
    }
    for (int &index: catAssign) {
        index = bcmAssign[index - 1];
    }

    Gpio gpio;
    Gpio::Group allGroup = gpio.select(bcmAssign);
    Gpio::Group anGroup = gpio.select(anAssign);
    Gpio::Group catGroup = gpio.select(catAssign);

    List<Gpio::Pin> an;
    List<Gpio::Pin> cat;

    for (int index: anAssign) {
        an << gpio[index];
    }
    for (int index: catAssign) {
        cat << gpio[index];
    }

    allGroup.setup(Gpio::Mode::Output);
    anGroup = Gpio::Level::Low;
    catGroup = Gpio::Level::High;

    String inputLine;
    String display;
    String dot = "    ";

    double lastTime = System::now();

    fout() <<
        "Press <Ctrl-C> to terminate\n"
        "> ";

    while (signalsReceived.count() == 0)
    {
        if (inputLine.count() == 0) {
            double nowTime = System::now();
            bool change = static_cast<uint64_t>(nowTime) / 60  != static_cast<uint64_t>(lastTime) / 60;
            if (change || display.count() == 0) {
                Date date = Timezone::date(nowTime);
                display = dec(date.hour(), 2) + dec(date.minutes(), 2);
                if (dot != " .  ") dot = " .  ";
                lastTime = nowTime;
            }
            bool blink = (static_cast<uint64_t>(nowTime) & 1) != 0;
            dot[1] = blink ? '.' : ' ';
        }
        else if (display.count() == 0) {
            fout() << "> ";
            inputLine.trimTrailing();
            if (inputLine.count() == 0) continue;
            dot = "    ";
            display = "";
            for (char ch: inputLine) {
                if (ch != '.') display += ch;
                else dot[display.count() - 1] = '.';
            }
            while (display.count() < 4) display += ' ';
            display.truncate(4);
        }

        for (int i = 0; i < 4; ++i)
        {
            const int displayTime = 4; // display duration of a single cell

            if (IoStream::input().wait(IoEvent::ReadyRead, displayTime)) {
                String buffer = String::allocate(64);
                buffer.truncate(IoStream::input().read(&buffer));
                inputLine = buffer;
                display = "";
                break;
            }

            catGroup = Gpio::Level::High; // blank all cells

            /** prepare glyph output
              */
            char ch = display[3 - i];
            uint8_t glyph = 0;

            if ('0' <= ch && ch <= '9') {
                glyph = number[ch - '0'];
            }
            else if ('A' <= ch && ch <= 'Z') {
                glyph = letter[ch - 'A'];
            }
            else if ('a' <= ch && ch <= 'z') {
                glyph = letter[ch - 'a'];
            }
            if (dot[3 - i] != ' ') {
                glyph |= 0b100;
            }
            for (int k = 0; k < 8; ++k) {
                Gpio::Level level = ((glyph & (1 << k)) != 0) ? Gpio::Level::High : Gpio::Level::Low;
                an[k] = level;
            }

            cat[i] = Gpio::Level::Low; // light up current cell
        }
    }

    anGroup = Gpio::Level::Low;
    catGroup = Gpio::Level::Low;
    allGroup.setup(Gpio::Mode::Input);

    signalMaster.wait();

    fout() << nl;

    return 0;
}
