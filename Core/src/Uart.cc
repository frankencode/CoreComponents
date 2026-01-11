#include <cc/Uart>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

namespace cc {

struct Uart::State final: public IoStream::State
{
    explicit State(const String &path, int speed):
        IoStream::State{::open(path, O_RDWR|O_NOCTTY|O_SYNC)},
        path_{path}
    {
        if (fd_ < 0) CC_SYSTEM_DEBUG_ERROR(errno);

        struct termios attr;

        /** get current settings
          */
        if (::tcgetattr(fd_, &attr) == -1) CC_SYSTEM_DEBUG_ERROR(errno);

        speed_t baud = B0;

        switch (speed) {
            case      50: baud =      B50; break;
            case      75: baud =      B75; break;
            case     110: baud =     B110; break;
            case     134: baud =     B134; break;
            case     150: baud =     B150; break;
            case     200: baud =     B200; break;
            case     300: baud =     B300; break;
            case     600: baud =     B600; break;
            case    1200: baud =    B1200; break;
            case    1800: baud =    B1800; break;
            case    2400: baud =    B2400; break;
            case    4800: baud =    B4800; break;
            case    9600: baud =    B9600; break;
            case   19200: baud =   B19200; break;
            case   38400: baud =   B38400; break;
            case   57600: baud =   B57600; break;
            case  115200: baud =  B115200; break;
            case  230400: baud =  B230400; break;
            case  460800: baud =  B460800; break;
            case  500000: baud =  B500000; break;
            case  576000: baud =  B576000; break;
            case  921600: baud =  B921600; break;
            case 1000000: baud = B1000000; break;
            case 1152000: baud = B1152000; break;
            case 1500000: baud = B1500000; break;
            case 2000000: baud = B2000000; break;
            default: CC_ASSERT(false /*unsupported baud rate*/);
        };

        /** disable all teletype input/output processing logic, especially canonical mode
          */
        attr.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON|IXOFF|IXANY);
        attr.c_oflag = ~OPOST;
        attr.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        attr.c_cflag &= ~PARENB;    // do not generate or check a parity bit
        attr.c_cflag &= ~CSTOPB;    // disable 2 stop bit option, 1 stop bit is enough
        attr.c_cflag &= ~CRTSCTS;   // disable hardware flow control

        /** set baud rate
          */
        if (::cfsetospeed(&attr, baud) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        if (::cfsetispeed(&attr, baud) == -1) CC_SYSTEM_DEBUG_ERROR(errno);

        /** set character size
          */
        attr.c_cflag &= ~CSIZE;     // unset character size mask
        attr.c_cflag |= CS8;        // set to 8-bit characters

        /** establish new settings
          */
        if (::tcsetattr(fd_, TCSAFLUSH, &attr) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    }

    ~State()
    {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    String path_;
};

Uart::Uart(const String &path, int speed):
    IoStream{new State{path, speed}}
{}

const String &Uart::path() const
{
    return me().path_;
}

const Uart::State &Uart::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
