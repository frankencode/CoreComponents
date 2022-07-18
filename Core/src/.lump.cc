/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DirWalker>
#include <cc/Dir>
#include <cc/File>

namespace cc {

struct DirWalker::State: public Object::State
{
    State() = default;

    State(const String &path):
        dir{path}
    {}

    bool read(Out<String> path, Out<bool> isDir)
    {
        if (child) {
            if (child().read(path, isDir))
                return true;
            if (deleteOrder) {
                path << child().dir.path();
                isDir << true;
                child = nullptr;
                return true;
            }
            child = nullptr;
        }

        for (String name; dir.read(&name);)
        {
            if (name == "") continue;
            if (ignoreHidden && name.startsWith('.')) continue;

            String h = dir.path() / name;
            try { child = new State{h}; }
            catch (...) { child = nullptr; }
            bool d = child;
            if (child) {
                if (depth != maxDepth) {
                    if (!followSymlink && File::readlink(h) != "") {
                        child = nullptr;
                    }
                    else {
                        child().maxDepth = maxDepth;
                        child().followSymlink = followSymlink;
                        child().deleteOrder = deleteOrder;
                        child().depth = depth + 1;
                        if (deleteOrder)
                            return read(path, isDir);
                    }
                }
                else {
                    child = nullptr;
                }
            }
            path << h;
            isDir << d;
            return true;
        }
        return false;
    }

    Dir dir;
    int maxDepth { -1 };
    bool ignoreHidden { false };
    bool followSymlink { false };
    bool deleteOrder { false };
    int depth { 0 };
    Handle<State> child { nullptr };
};

DirWalker::DirWalker(const String &path):
    Object{new State{path}}
{}

bool DirWalker::read(Out<String> path, Out<bool> isDir)
{
    return me().read(path, isDir);
}

void DirWalker::setMaxDepth(int depth)
{
    me().maxDepth = depth;
}

void DirWalker::setIgnoreHidden(bool on)
{
    me().ignoreHidden = on;
}

void DirWalker::setFollowSymlink(bool on)
{
    me().followSymlink = on;
}

void DirWalker::setDeleteOrder(bool on)
{
    me().deleteOrder = on;
}

DirWalker::State &DirWalker::me()
{
    return Object::me.as<State>();
}

const DirWalker::State &DirWalker::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TransferLimiter>

namespace cc {

struct TransferLimiter::State: public Stream::State
{
    State(const Stream &stream, long long readLimit, long long writeLimit):
        stream_{stream},
        readLimit_{readLimit},
        writeLimit_{writeLimit}
    {}

    bool waitEstablished(int timeout) override
    {
        return stream_.waitEstablished(timeout);
    }

    bool wait(IoEvent event, int timeout) override
    {
        return stream_.wait(event, timeout);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        long n = stream_.read(buffer, maxFill);
        totalRead_ += n;
        if (readLimit_ > 0 && totalRead_ > readLimit_) {
            throw InputExhaustion{};
        }
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        totalWritten_ += fill >= 0 ? fill : buffer.count();
        if (writeLimit_ > 0 && totalWritten_ > writeLimit_) {
            throw OutputExhaustion{};
        }
        stream_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        for (const Bytes &buffer: buffers) {
            totalWritten_ += buffer.count();
        }
        if (writeLimit_ > 0 && totalWritten_ > writeLimit_) {
            throw OutputExhaustion{};
        }
        stream_.write(buffers);
    }

    Stream stream_;
    long long readLimit_ { -1 };
    long long writeLimit_ { -1 };
    long long totalRead_ { 0 };
    long long totalWritten_ { 0 };
};

TransferLimiter::TransferLimiter(const Stream &stream, long long readLimit, long long writeLimit):
    Stream{new TransferLimiter::State{stream, readLimit, writeLimit}}
{}

TransferLimiter::TransferLimiter(State *newState):
    Stream{newState}
{}

long long TransferLimiter::readLimit() const
{
    return me().readLimit_;
}

long long TransferLimiter::writeLimit() const
{
    return me().writeLimit_;
}

long long TransferLimiter::totalRead() const
{
    return me().totalRead_;
}

long long TransferLimiter::totalWritten() const
{
    return me().totalWritten_;
}

void TransferLimiter::reset()
{
    me().totalRead_ = 0;
    me().totalWritten_ = 0;
}

const TransferLimiter::State &TransferLimiter::me() const
{
    return Object::me.as<State>();
}

TransferLimiter::State &TransferLimiter::me()
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Version>
#include <cc/input>
#include <cc/str>

namespace cc {

Version::Version(const String &s)
{
    List<String> parts = s.trimmed("v").split('.');
    if (parts.count() > 0) me().major = readNumber<int>(parts.at(0));
    if (parts.count() > 1) me().minor = readNumber<int>(parts.at(1));
    if (parts.count() > 2) me().patch = readNumber<int>(parts.at(2));
}

String Version::toString() const
{
    return String{
        List<String>{} << str(major()) << str(minor()) << str(patch()),
        "."
    };
}

const TypeInfo VariantType<Version>::info
{
    .typeName = "Version",
    .str = [](const void *bytes) {
        return VariantType<Version>::retrieve(bytes).toString();
    },
    .cleanup = [](void *bytes){},
    .assign = [](void *dst, const void *src) {
        new(dst)Version{*static_cast<const Version *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<Version>::retrieve(a) == VariantType<Version>::retrieve(b);
    }
};

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Date>
#include <cc/System>
#include <cc/Timezone>
#include <cc/str>
#include <limits>
#include <cmath>
#include <cstdint>

namespace cc {

/* Every 4th year is a leap year excluding every last year in the
 * 100 year cycle (century) but not the last year in a 400 year cycle.
 */
bool leapYear(int y)
{
    return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
}

/** Number of leap years between x and y (excluding x, y).
  */
static int leaps(int x, int y)
{
    if (x > y) {
        int h = x;
        x = y;
        y = h;
    }
    --y; ++x; // exclude year x, y
    if (x > y) return 0;
    return y / 4 - y / 100 + y / 400 - (x - 1) / 4 + (x - 1) / 100 - (x - 1) / 400;
}

/** Days in a year are arbitrarily distributed but weighted to make the
  * average length of a year match the length of the solar year.
  * Interestingly the length of the year is measured in days, which is a duration
  * depending on Earth's angular velocity.
  */
int daysInYear(int y)
{
    return 31 * 7 + 30 * 4 + 28 + leapYear(y);
}

int daysInMonth(int i, int y)
{
    const int days[] = {
        31/*Jan*/, 28/*Feb*/, 31/*Mar*/, 30/*Apr*/, 31/*May*/, 30/*Jun*/,
        31/*Jul*/, 31/*Aug*/, 30/*Sep*/, 31/*Oct*/, 30/*Nov*/, 31/*Dec*/
    };
    return days[i] + (i == 1) * leapYear(y);
}

void Date::State::timeToCalendar()
{
    const int C1   = 365;
    const int C4   =  4 * C1 + 1;
    const int C100 = 25 * C4 - 1;
    const int C400 =  4 * C100 + 1;
    const int N0   = 719528; // linear day number of Jan 1st year 0

    std::int64_t t = std::int64_t(time) + offset + std::int64_t(N0) * 86400;
    int n = t / 86400;
    if (n < 0) n = 0;

    hour = (t / 3600) % 24;
    minutes = (t / 60) % 60;
    seconds = t % 60;
    weekDay = (n + 6) % 7;

    int y = 400 * (n / C400);
    n = n % C400;

    while (true) {
        int h = C100 + leapYear(y);
        if (n < h) break;
        n -= h;
        y += 100;
    }

    while (true) {
        int h = C4 - !leapYear(y);
        if (n < h) break;
        n -= h;
        y += 4;
    }

    while (true) {
        int h = C1 + leapYear(y);
        if (n < h) break;
        n -= h;
        ++y;
    }

    year = y;
    yearDay = n;

    month = 0;
    while (true) {
        int h = daysInMonth(month, y);
        if (n < h) break;
        ++month;
        n -= h;
    }
    ++month;
    day = n + 1;
}

void Date::State::calendarToTime()
{
    if (year < 1) year = 1;
    if (month > 12) month = 12;
    else if (month < 1) month = 1;
    if (day > 31) day = 31;
    else if (day < 1) day = 1;
    if (hour < 0) hour = 0;
    else if (hour > 23) hour = 23;
    if (minutes > 59) minutes = 59;
    else if (minutes < 0) minutes = 0;
    if (seconds > 59) seconds = 59;
    else if (seconds < 0) seconds = 0;

    --day;
    --month;

    int64_t t = 0;
    for (int i = 0; i < month; ++i)
        t += daysInMonth(i, year);
    t += day;
    yearDay = t;
    t += (31 * 7 + 30 * 4 + 28) * (year - 1970);
    if (year >= 1970)
        t += leaps(1970, year);
    else
        t -= leaps(1970, year - 1);
    weekDay = (719528 + t + 6) % 7;
    t *= 86400;
    t += 3600 * hour + 60 * minutes + seconds;
    time = t - offset;

    ++day;
    ++month;
}

Date Date::now()
{
    return Date{System::now()};
}

Date Date::localNow()
{
    return local(System::now());
}

Date Date::local(double time)
{
    return Date{time, Timezone::offset(time)};
}

Date::Date():
    me{std::numeric_limits<double>::quiet_NaN()}
{}

Date::Date(double time, int offset):
    me{time, offset}
{
    me().timeToCalendar();
}

Date::Date(int year, int month, int day, int hour, int minutes, int seconds, int offset):
    me{0., offset, year, month, day, hour, minutes, seconds}
{
    me().calendarToTime();
}

bool Date::isValid() const
{
    return !std::isnan(me().time);
}

double Date::nextTime(int hour, int minutes, int seconds) const
{
    assert(offset() == Timezone::offset(time()));

    Date n {
        year(),
        month(),
        day(),
        hour,
        minutes,
        seconds,
        offset()
    };

    double t = n.time();
    if (n <= *this) t += SecondsPerDay;
    t += n.offset() - Timezone::offset(t);

    return t;
}

String Date::monthName() const
{
    const char *names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    int i = month() - 1;
    if (i < 0) i = 0;
    else if (i > 11) i = 11;
    return names[i];
}

String Date::dayName() const
{
    const char *names[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    int i = weekDay();
    if (i < 0) i = 0;
    else if (i > 6) i = 6;
    return names[i];
}

String Date::toString() const
{
    String tz = "Z";
    int offset = me().offset / 60;
    if (offset > 0)
        tz = List<String>{} << "+" << dec(offset / 60, 2) << dec(offset % 60, 2);
    else if (offset < 0)
        tz = List<String>{} << "-" << dec((-offset) / 60, 2) << dec((-offset) % 60, 2);

    return List<String>{}
        << dec(me().year, 4) << "-" << dec(me().month, 2) << "-" << dec(me().day, 2)
        << "T" << dec(me().hour, 2) << dec(me().minutes, 2) << dec(me().seconds, 2) << tz;
}

} // namespace cc
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Timezone>
#include <cc/File>
#include <cc/ByteSource>
#include <cc/Layout>
#include <cc/Singleton>

namespace cc {

struct Timezone::State final: public Object::State
{
    explicit State(const String &data)
    {
        ByteSource source{data, ByteOrder::BigEndian};

        String magic = String::allocate(4);
        source.read(&magic);
        if (magic != "TZif") {
            throw FormatError{data, 0, "Not a timezone information file (expected magic \"TZif\")"};
        }

        {
            char ch = '\0';
            source.readChar(&ch);
            if (ch != '\0') {
                version_ = ch - '0';
            }
        }

        if (version_ >= 2) {
            long newOffset = source.currentOffset();
            if (data.find("TZif", 4, &newOffset)) {
                source = ByteSource{String{data}.selectTail(data.count() - newOffset), ByteOrder::BigEndian};
                source.readUInt32();
                char ch = '\0';
                source.readChar(&ch);
                if (ch != '\0') {
                    version_ = ch - '0';
                }
            }
        }

        source.readSpan(15); // reserved

        /*uint32_t utLocalCount =*/ source.readUInt32();
        /*uint32_t stdWallCount =*/ source.readUInt32();
        /*uint32_t leapCount =*/ source.readUInt32();
        uint32_t timeCount = source.readUInt32();
        uint32_t typeCount = source.readUInt32();
        uint32_t charCount = source.readUInt32();

        auto transitionTimes = Array<double>::allocate(timeCount);

        for (uint32_t i = 0; i < timeCount; ++i)
        {
            if (version_ >= 2)
                transitionTimes[i] = source.readInt64();
            else
                transitionTimes[i] = source.readInt32();
        }

        auto typeIndex = Array<int>::allocate(timeCount);

        for (uint32_t i = 0; i < timeCount; ++i)
        {
            typeIndex[i] = source.readUInt8();
        }

        auto timeData = Array<TimeData>::allocate(typeCount);

        for (uint32_t i = 0; i < typeCount; ++i)
        {
            TimeData &data = timeData.at(i);
            data.utcOffset = source.readInt32();
            data.isDaylightSaving = source.readUInt8();
            data.zoneOffset = source.readUInt8();
        }

        String zones = String::allocate(charCount);
        source.read(&zones);

        TimeInfo pred;

        for (uint32_t i = 0; i < timeCount; ++i) {
            int j = typeIndex[i];
            if (timeData.count() <= j) throw FormatError{data, source.currentOffset(), "Invalid type index"};
            const TimeData &data = timeData.at(typeIndex[i]);
            TimeInfo info{New{}};
            info->offset_ = data.utcOffset;
            info->timezone_ = String{zones.chars() + data.zoneOffset};
            info->isDaylightSaving_ = data.isDaylightSaving;
            info->startTime_ = transitionTimes[i];

            if (i > 0) {
                double extent = transitionTimes[i] - transitionTimes[i - 1];
                timeLayout_.pushBack(pred, extent);
            }

            pred = info;
        }

        timeEnd_ = pred;
    }

    double startTime() const
    {
        return timeLayout_.at(0).item().startTime();
    }

    double endTime() const
    {
        return timeLayout_.at(timeLayout_.count() - 1).item().startTime();
    }

    const TimeInfo &infoForTime(double time) const
    {
        if (time < startTime()) return timeLayout_.at(0).item();
        else if (endTime() <= time) return timeEnd_;
        return timeLayout_.find(time - startTime()).item();
    }

    struct TimeData {
        int32_t utcOffset;
        bool isDaylightSaving;
        int zoneOffset;
    };

    int version_ { 0 };
    Layout<TimeInfo, double> timeLayout_;
    TimeInfo timeEnd_;
};

class Timezone::LocalCache final: public Singleton
{
public:
    LocalCache():
        Singleton{instance<State>()}
    {}

    const Timezone &localZone() const { return me().localZone_; }

private:
    struct State final: public Singleton::State
    {
        State() {
            localZone_ = Timezone::loadFromFile("/etc/localtime");
        }

        Timezone localZone_;
    };

    const State &me() const { return Object::me.as<State>(); }
};

Timezone Timezone::local()
{
    return LocalCache{}.localZone();
}

Timezone Timezone::loadFromFile(const String &path)
{
    return Timezone{File{path, FileOpen::ReadOnly}.map()};
}

Timezone::Timezone(const String &data):
    Object{new State{data}}
{}

long Timezone::count() const
{
    return me().timeLayout_.count();
}

const TimeInfo &Timezone::at(long i) const
{
    return me().timeLayout_.at(i).item();
}

const TimeInfo &Timezone::infoForTime(double time) const
{
    return me().infoForTime(time);
}

int Timezone::operator()(double time) const
{
    return me().infoForTime(time).offset();
}

Timezone::State &Timezone::me()
{
    return Object::me.as<State>();
}

const Timezone::State &Timezone::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PseudoTerminal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // open
#include <unistd.h> // close
#include <stdlib.h> // posix_openpt, grantpt, unlockpt, ptsname

namespace cc {

PseudoTerminal::State::State()
{
    fd_ = ::posix_openpt(O_RDWR|O_NOCTTY);
    if (fd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::grantpt(fd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::unlockpt(fd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    slaveFd_ = ::open(::ptsname(fd_), O_RDWR|O_NOCTTY);
    if (slaveFd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

PseudoTerminal::State::~State()
{
    if (slaveFd_ != -1) ::close(slaveFd_);
}

void PseudoTerminal::State::onStart()
{
    if (slaveFd_ != -1) {
        ::close(slaveFd_);
        slaveFd_ = -1;
    }
}

PseudoTerminal::PseudoTerminal():
    IoChannel{new PseudoTerminal::State}
{}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf8Source>

namespace cc {

String Utf8Source::DecodingError::message() const
{
    return "UTF-8 error: failed to decode input bytes";
}

char32_t Utf8Source::readMultiByte(char32_t ch)
{
    int n = -1; // number of additional bytes

    // translate prefix to code length (n is the number of successive bytes)
    if ((ch >> 5) == 6) { // code prefix: 6 = (110)2
        ch = ch & 0x1F;
        n = 1;
    }
    else if ((ch >> 4) == 14) { // code prefix: 14 = (1110)2
        ch = ch & 0x0F;
        n = 2;
    }
    else if ((ch >> 3) == 30) { // code prefix: 30 = (11110)2
        ch = ch & 0x07;
        n = 3;
    }

    int n2 = n;

    // read n successive characters (chs), which carry the code prefix (10)2
    while (n > 0) {
        uint8_t chs = byteSource_.readUInt8();
        if ((chs >> 6) == 2) // 2 = (10)2
            ch = (ch << 6) | (chs & 0x3F);
        else
            break;
        --n;
    }

    // enforce minimum code length
    if (n == 0) {
        if (n2 == 1) {
            if (ch < 0x80) n = -1;
        }
        else if (n2 == 2) {
            if (ch < 0x800) n = -1;
        }
        else if (n2 == 3) {
            if (ch < 0x10000) n = -1;
        }
    }

    if (n < 0) throw DecodingError{};

    return ch;
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SignalMaster>
#include <cc/Thread>
#include <cc/SystemError>

namespace cc {

struct SignalMaster::State: public Object::State
{
    State(const Channel<Signal> &signals, const SignalSet &shutdownSet):
        signals_{signals},
        shutdownSet_{shutdownSet}
    {}

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void wait()
    {
        thread_.wait();
    }

    void kill(Signal signal)
    {
        thread_.kill(signal);
    }

    void run()
    {
        SignalSet all = SignalSet::full();
        while (true) {
            int signal = 0;
            int error = ::sigwait(all, &signal);
            if (error != 0) CC_SYSTEM_DEBUG_ERROR(error);
            signals_.pushBack(static_cast<Signal>(signal));
            if (shutdownSet_.contains(static_cast<Signal>(signal))) break;
        }
    }

    Channel<Signal> signals_;
    SignalSet shutdownSet_;
    Thread thread_;
};

SignalSet SignalMaster::defaultShutdownSignals()
{
    SignalSet set = SignalSet::empty();
    set.insert(Signal::Interrupt);
    set.insert(Signal::Terminate);
    return set;
}

SignalMaster::SignalMaster(const Channel<Signal> &signals, const SignalSet &shutdownSet):
    Object{new State{signals, shutdownSet}}
{}

void SignalMaster::start()
{
    me().start();
}

void SignalMaster::wait()
{
    me().wait();
}

void SignalMaster::kill(Signal signal)
{
    me().kill(signal);
}

SignalMaster::State &SignalMaster::me()
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Signal>
#include <cc/Format>

namespace cc {

String str(Signal signal)
{
    String s;
    switch (signal) {
        case Signal::Abort     : s = "SIGABRT"; break;
        case Signal::Alarm     : s = "SIGALRM"; break;
        case Signal::Bus       : s = "SIGBUS"; break;
        case Signal::Child     : s = "SIGCHLD"; break;
        case Signal::Continue  : s = "SIGCONT"; break;
        case Signal::FPE       : s = "SIGFPE"; break;
        case Signal::HangUp    : s = "SIGHUP"; break;
        case Signal::Interrupt : s = "SIGINT"; break;
        case Signal::Kill      : s = "SIGKILL"; break;
        case Signal::Pipe      : s = "SIGPIPE"; break;
        case Signal::Quit      : s = "SIGQUIT"; break;
        case Signal::SegV      : s = "SIGSEGV"; break;
        case Signal::Stop      : s = "SIGSTOP"; break;
        case Signal::Terminate : s = "SIGTERM"; break;
        case Signal::User1     : s = "SIGUSR1"; break;
        case Signal::User2     : s = "SIGUSR2"; break;
        case Signal::WindowSize: s = "SIGWINCH"; break;
        case Signal::Undefined : s = "SIGUNDEF"; break;
        default                      : s = Format{"SIG%%"} << +signal; break;
    };
    return s;
}

Signaled::Signaled(Signal signal):
    signal_{signal}
{}

String Signaled::message() const
{
    return Format{} << "Caught " << signal_;
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MemoryStream>

namespace cc {

struct MemoryStream::State: public Stream::State
{
    State(const Bytes &buffer):
        buffer_{buffer}
    {}

    long read(Out<Bytes> data, long maxFill = -1) override
    {
        if (i_ == buffer_.count()) return 0;

        if (maxFill < 0) maxFill = data->count();
        if (maxFill == 0) return 0;

        const long n = (i_ + maxFill <= buffer_.count()) ? maxFill : buffer_.count() - i_;

        buffer_.copyRangeTo(i_, i_ + n, &data);

        i_ += n;
        return n;
    }

    void write(const Bytes &data, long fill = -1) override
    {
        if (i_ == buffer_.count()) return;

        if (fill < 0) fill = data.count();
        if (fill == 0) return;

        const long n = (i_ + fill <= buffer_.count()) ? fill : buffer_.count() - i_;

        data.copyRangeToOffset(0, n, &buffer_, i_);

        i_ += n;
    }

    Bytes buffer_;
    long i_ { 0 };
};

MemoryStream::MemoryStream(const Bytes &buffer):
    Stream{new State{buffer}}
{}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Variant>
#include <cc/input>
#include <cc/str>

namespace cc {

const TypeInfo VariantType<void>::info
{
    .typeName = "void",
    .str = [](const void *bytes) { return String{}; }
};

const TypeInfo VariantType<long>::info
{
    .typeName = "long",
    .str = [](const void *bytes) { return str(VariantType<long>::retrieve(bytes)); },
    .equal = [](const void *a, const void *b) {
        return VariantType<long>::retrieve(a) == VariantType<long>::retrieve(b);
    }
};

const TypeInfo VariantType<bool>::info
{
    .typeName = "bool",
    .str = [](const void *bytes) { return str(VariantType<bool>::retrieve(bytes)); },
    .equal = [](const void *a, const void *b) {
        return VariantType<bool>::retrieve(a) == VariantType<bool>::retrieve(b);
    }
};

const TypeInfo VariantType<double>::info
{
    .typeName = "double",
    .str = [](const void *bytes) { return str(VariantType<double>::retrieve(bytes)); },
    .equal = [](const void *a, const void *b) {
        return VariantType<double>::retrieve(a) == VariantType<double>::retrieve(b);
    }
};

const TypeInfo VariantType<String>::info {
    .typeName = "String",
    .str = [](const void *bytes) {
        return String{VariantType<String>::retrieve(bytes)};
    },
    .cleanup = [](void *bytes){
        static_cast<String *>(bytes)->~String();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)String{*static_cast<const String *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<String>::retrieve(a) == VariantType<String>::retrieve(b);
    }
};

const TypeInfo VariantType<List<Variant>>::info {
    .typeName = "List<Variant>",
    .str = [](const void *bytes) {
        const List<Variant> &list = *static_cast<const List<Variant> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            Variant value = list.at(pos);
            if (value.is<String>()) parts << "\"";
            parts << value;
            if (value.is<String>()) parts << "\"";
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<Variant> *>(bytes)->~List<Variant>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<Variant>{*static_cast<const List<Variant> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<Variant>>::retrieve(a) == VariantType<List<Variant>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<String>>::info {
    .typeName = "List<String>",
    .str = [](const void *bytes) {
        const List<String> &list = *static_cast<const List<String> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        return String{"[ \"" + String{list, "\", \""} + "\" ]"};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<String> *>(bytes)->~List<String>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<String>{*static_cast<const List<String> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<String>>::retrieve(a) == VariantType<List<String>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<bool>>::info {
    .typeName = "List<bool>",
    .str = [](const void *bytes) {
        const List<bool> &list = *static_cast<const List<bool> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<bool> *>(bytes)->~List<bool>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<bool>{*static_cast<const List<bool> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<bool>>::retrieve(a) == VariantType<List<bool>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<long>>::info {
    .typeName = "List<long>",
    .str = [](const void *bytes) {
        const List<long> &list = *static_cast<const List<long> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<long> *>(bytes)->~List<long>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<long>{*static_cast<const List<long> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<long>>::retrieve(a) == VariantType<List<long>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<double>>::info {
    .typeName = "List<double>",
    .str = [](const void *bytes) {
        const List<double> &list = *static_cast<const List<double> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<double> *>(bytes)->~List<double>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<double>{*static_cast<const List<double> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<double>>::retrieve(a) == VariantType<List<double>>::retrieve(b);
    }
};

const TypeInfo VariantType<Map<String, Variant>>::info {
    .typeName = "Map<String,Variant>",
    .str = [](const void *bytes) {
        const Map<String, Variant> &map = *static_cast<const Map<String, Variant> *>(bytes);
        List<String> parts;
        parts << "{ ";
        for (Locator pos = map.head(); pos; ++pos) {
            Variant value = map.at(pos).value();
            parts << "\"" << map.at(pos).key() << "\": ";
            if (value.is<String>()) parts << "\"";
            parts << str(value);
            if (value.is<String>()) parts << "\"";
            if (+pos != map.count() - 1)
                parts << ", ";
        }
        parts << " }";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<Map<String, Variant> *>(bytes)->~Map<String, Variant>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)Map<String, Variant>{*static_cast<const Map<String, Variant> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<Map<String, Variant>>::retrieve(a) == VariantType<Map<String, Variant>>::retrieve(b);
    }
};

void VariantType<Map<String, Variant>>::store(void *bytes, const Map<String, Variant> &x)
{
    static_assert(sizeof(Map<String, Variant>) <= 8);

    new(bytes)Map<String, Variant>{x};
}

Map<String, Variant> VariantType<Map<String, Variant>>::retrieve(const void *bytes, const TypeInfo &typeInfo)
{
    assert((&typeInfo == &VariantType<Map<String, Variant>>::info)); // expected type 'Map<String, Variant>'

    return *static_cast<const Map<String, Variant> *>(bytes);
}

Variant Variant::read(const String &s)
{
    if (s.startsWith('"') && s.endsWith('"'))
        return Variant{s.copy(1, s.count() - 1)};

    if (s.startsWith('[') && s.endsWith(']')) {
        List<String> sl = s.copy(1, s.count() - 1).split(',');
        List<Variant> vl;
        for (String &x: sl) {
            x.trim();
            vl.append(Variant::read(x));
        }
        return Variant{vl};
    }

    if (
        s.find('.') ||
        (!s.startsWith("0x") && (s.find('e') || s.find('E')))
    ) {
        double value = 0;
        if (scanNumber<double>(s, &value) == s.count())
            return Variant{value};
    }

    long value = 0;
    if (scanNumber<long>(s, &value) == s.count()) return Variant{value};
    if (s == "true" || s == "on") return Variant{true};
    if (s == "false" || s == "off") return Variant{false};

    return Variant{s};
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Process>
#include <cc/Command>
#include <cc/File>
#include <sys/wait.h> // waitpid
#include <sys/stat.h> // umask
#include <errno.h> // errno
#include <unistd.h> // chdir, alarm, __MACH__?
#include <stdlib.h> // getenv, setenv, malloc, free
#ifdef __MACH__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#include <crt_externs.h> // _NSGetEnviron
#endif

#ifndef __MACH__
extern "C" char **environ;
#endif

namespace cc {

Process::State::~State()
{
    wait();
}

int Process::State::wait()
{
    if (pid_ < 0) return exitStatus_;

    int ret = 0;
    if (::waitpid(pid_, &ret, 0) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    pid_ = -1;

    if (WIFSIGNALED(ret))
        return exitStatus_ = -WTERMSIG(ret);

    return exitStatus_ = WEXITSTATUS(ret);
}

Process::Process(const String &command):
    Process{Command{command}}
{}

Process::Process(const Command &command):
    Object{new State}
{
    Command{command}.start(*this);
}

int Process::exec(const String &command)
{
    Process process {
        Command{command}
        .io(0, IoStream::input())
        .io(1, IoStream::output())
        .io(2, IoStream::error())
    };

    return process.wait();
}

int Process::id() const
{
    return me().pid_;
}

int Process::wait()
{
    return me().wait();
}

IoStream Process::input() const
{
    return me().standardStreams_[0];
}

IoStream Process::output() const
{
    return me().standardStreams_[1];
}

IoStream Process::error() const
{
    return me().standardStreams_[2];
}

void Process::kill(int pid, Signal signal)
{
    if (::kill(pid, +signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::killGroup(int gid, Signal signal)
{
    if (::kill(-gid, +signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::raise(Signal signal)
{
    if (::raise(+signal) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::exit(int status)
{
    ::exit(status);
}

int Process::currentId()
{
    return ::getpid();
}

int Process::parentId()
{
    return ::getppid();
}

String Process::execPath()
{
    String path;
    #if defined __linux || defined __CYGWIN32__ || defined __CYGWIN__
    path = File::readlink("/proc/self/exe");
    #elif __OpenBSD__
    path = File::readlink("/proc/curproc/file");
    #elif __NetBSD__
    path = File::readlink("/proc/curproc/exe")
    #elif __MACH__
    char *buf = 0;
    uint32_t bufSize = 0;
    _NSGetExecutablePath(buf, &bufSize);
    buf = (char *)::malloc(bufSize + 1);
    memset(buf, 0, bufSize + 1);
    _NSGetExecutablePath(buf, &bufSize);
    path = buf;
    ::free(buf);
    #else
    #warning "Missing implementation for Process::execPath()"
    #endif
    return path;
}

String Process::env(const String &name)
{
    return ::getenv(name);
}

String Process::env(const String &name, const String &fallback)
{
    String value = ::getenv(name);
    if (value == "") value = fallback;
    return value;
}

void Process::setEnv(const String &name, const String &value)
{
    if (::setenv(name, value, 1) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::unsetEnv(const String &name)
{
    if (::unsetenv(name) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

Map<String> Process::envMap()
{
    char **env = environ();
    Map<String> map;
    for (int i = 0; env[i]; ++i) {
        String s{env[i]};
        long j = 0;
        if (s.find('=', &j)) {
            String value = s.select(j + 1, s.count());
            s.truncate(j);
            map.insert(s, value);
        }
    }
    return map;
}

String Process::cwd()
{
    int size = 0x1000;
    char *buf = (char *)::malloc(size);
    char *ret = 0;
    while (true) {
        ret = ::getcwd(buf, size);
        if (ret) break;
        if (errno == ERANGE) {
            ::free(buf);
            size += 0x1000;
            buf = (char *)::malloc(size);
        }
        else
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    String path{ret};
    ::free(buf);
    return path;
}

void Process::cd(const String &path)
{
    if (::chdir(path) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setUserMask(FileMode newMask, Out<FileMode> oldMask)
{
    auto h = ::umask(+newMask);
    oldMask << static_cast<FileMode>(h);
}

int Process::realUserId()
{
    return ::getuid();
}

int Process::realGroupId()
{
    return ::getgid();
}

int Process::effectiveUserId()
{
    return ::geteuid();
}

int Process::effectiveGroupId()
{
    return ::getegid();
}

bool Process::isSuperUser()
{
    return ::geteuid() == 0 || ::getegid() == 0;
}

void Process::setUserId(int uid)
{
    if (::setuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setGroupId(int gid)
{
    if (::setgid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveUserId(int uid)
{
    if (::seteuid(uid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::setEffectiveGroupId(int gid)
{
    if (::setegid(gid) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void Process::daemonize()
{
    pid_t pid = ::fork();
    if (pid == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (pid != 0) ::exit(0);
    ::setsid();
    ::umask(0);
    ::close(0);
    ::close(1);
    ::close(2);
}

bool Process::isDaemonized()
{
    return ::getsid(0) == ::getpgid(0) && ::isatty(0) != 1;
}

void Process::startSession()
{
    if (::setpgid(0, 0) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

char **&Process::environ()
{
    return
    #ifdef __MACH__
        *_NSGetEnviron();
    #else
        ::environ;
    #endif
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ByteSource>

namespace cc {

String ByteSource::readCString()
{
    List<char> chars;
    for (char ch; readChar(&ch);) {
        if (!ch) break;
        chars.append(ch);
    }
    String s = String::allocate(chars.count());
    long i = 0;
    for (char ch: chars) {
        s[i++] = ch;
    }
    return s;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ReplaySource>

namespace cc {

struct ReplaySource::State: public Stream::State
{
    State(const Bytes &buffer):
        recording_{buffer}
    {}

    long read(Out<Bytes> buffer, long maxFill) override
    {
        long avail = recording_.count() - i0_;
        if (avail == 0) return 0;

        if (maxFill < 0) maxFill = buffer->count();
        if (maxFill == 0) return 0;

        if (maxFill > avail) maxFill = avail;

        recording_.copyRangeTo(i0_, i0_ + maxFill, &buffer);
        i0_ += maxFill;

        return maxFill;
    }

    Bytes recording_;
    long i0_ { 0 };
};

ReplaySource::ReplaySource(const Bytes &buffer):
    Stream{new ReplaySource::State{buffer}}
{}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/input>

namespace cc {

long scanToken(const String &text, Out<String> token, const char *lt, long i0, long i1)
{
    if (i1 < 0 || i1 > text.count()) i1 = text.count();
    if (i0 > i1) i0 = i1;
    long i = i0;
    for (; i < i1; ++i) {
        const char *p = lt;
        if (!text.at(i)) break;
        for(; *p; ++p) {
            if (text.at(i) == *p) break;
        }
        if (*p) break;
    }
    token = text.copy(i0, i);
    return i;
}

bool offsetToLinePos(const String &text, long offset, Out<long> line, Out<long> pos)
{
    bool valid = true;
    if (offset < 0) {
        valid = false;
        offset = 0;
    }
    if (text.count() <= offset) {
        valid = false;
        offset = text.count();
    }
    long y = 1, x = 0;
    for (long i = 0; i < offset; ++i) {
        if (text.at(i) == '\n') {
            ++y;
            x = 0;
        }
        else {
            ++x;
        }
    }
    line = y;
    pos = x;
    return valid;
}

bool linePosToOffset(const String &text, long line, long pos, Out<long> offset)
{
    if (line <= 0) return false;

    long i = 0, n = text.count();

    for (long y = 1; y < line && i < n; ++i) {
        if (text.at(i) == '\n') ++y;
    }

    if (i + pos >= n) return false;

    offset = i + pos;
    return true;
}

String escape(const String& text)
{
    List<String> parts;
    long i = 0, i0 = 0;
    long n = text.count();
    for (; i < n; ++i) {
        unsigned char ch = text.template item<unsigned char>(i);
        if (ch < 32 || 127 <= ch) {
            if (i0 < i) parts.append(text.copy(i0, i));
            i0 = i + 1;
            if (ch == 0x08) parts.append("\\b");
            else if (ch == 0x09) parts.append("\\t");
            else if (ch == 0x0A) parts.append("\\n");
            else if (ch == 0x0D) parts.append("\\r");
            else if (ch == 0x0C) parts.append("\\f");
            else {
                String s = "\\xXX";
                const char *hex = "0123456789ABCDEF";
                s[+s - 2] = hex[ch / 16];
                s[+s - 1] = hex[ch % 16];
                parts.append(s);
            }
        }
    }

    if (parts.count() == 0) return text.copy();

    if (i0 < i) parts.append(text.copy(i0, i));

    return parts;
}

String normalize(const String &text, bool nameCase)
{
    String b = text.copy();
    for (long i = 0; i < b.count(); ++i) {
        if (b.item<unsigned char>(i) < 32)
            b.item<unsigned char>(i) = 32;
    }
    List<String> parts = b.split(" ");
    for (long i = 0; i < parts.count(); ++i) {
        String &s = parts[i];
        if (s.count() == 0) {
            parts.removeAt(i);
        }
        else {
            if (nameCase) {
                s = s.downcased();
                s[0] = cc::toUpper(s[0]);
            }
            ++i;
        }
    }
    return String{parts, " "};
}

String xmlEscape(const String &text)
{
    String h = text;
    if (h.find('<')) h.replace("<", "&lt;");
    if (h.find('>')) h.replace(">", "&gt;");
    if (h.find('&')) h.replace("?", "&amp;");
    return h;
}

String xmlSanitize(const String &text)
{
    List<String> parts;
    long i = 0, j = 0;
    long n = text.count();
    while (i < n) {
        char ch = text.at(i);
        if (ch == '<') {
            if (j < i) parts.append(text.copy(j, i));
            for (; i < n; ++i) if (text.at(i) == '>') break;
            i += (i != n);
            j = i;
        }
        else if (ch == '&') {
            if (j < i) parts.append(text.copy(j, i));
            for (; i < n; ++i) if (text.at(i) == ';') break;
            i += (i != n);
            j = i;
        }
        else {
            ++i;
        }
    }
    if (j < i) parts.append(text.copy(j, i));
    return parts;
}

String indent(const String &text, const String &prefix)
{
    return prefix + String{text.trailingTrimmed().split('\n'), prefix + "\n"};
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Format>

namespace cc {

NewLine nl;
Flush flush;

Format::Format(const String &pattern, const Stream &stream):
    me{stream, stream && stream.isDiscarding()}
{
    if (me().discardMode) return;
    String p = pattern;
    long i = 0, n = 0;
    while (true) {
        long i0 = i;
        bool found = p.find("%%", &i);
        if (i0 < i) (*this).append(p.select(i0, i));
        if (!found) break;
        long j = (*this).count() + n;
        me().placeHolder.pushBack(j);
        ++n;
        i += 2;
    }
    me().injectionMode = me().placeHolder.count() > 0;
}

Format::Format(const Stream &stream):
    me{stream, stream && stream.isDiscarding()}
{}

Format::~Format()
{
    try {
        flush();
    }
    catch (...)
    {}
}

void Format::flush()
{
    if (me().discardMode) return;
    if (me().stream && (*this).count() > 0) {
        if (me().injectionMode && me().lastInsert.count() != 0) {
            while (me().placeHolder.count() > 0) {
                int j = 0;
                me().placeHolder.popFront(&j);
                (*this).insertAt(j, me().lastInsert);
            }
        }
        me().stream.write(String{*this});
        (*this).deplete();
    }
}

Format &Format::operator<<(const Bytes &s)
{
    if (me().discardMode) return *this;

    int j = (*this).count();

    if (me().injectionMode) {
        if (me().placeHolder.count() > 0) {
            me().placeHolder.popFront(&j);
            me().lastPosition = j;
        }
        else {
            j = ++me().lastPosition;
        }
        (*this).insertAt(j, s);
        me().lastInsert = s;
    }
    else {
        (*this).append(s);
    }

    return *this;
}

Format &Format::operator<<(const NewLine &)
{
    if (me().discardMode) return *this;
    (*this).append(String{"\n", 1});
    return *this;
}

Format &Format::operator<<(const Flush &)
{
    if (me().discardMode) return *this;
    flush();
    return *this;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/String>
#include <cc/Utf8Sink>
#include <cc/Casefree>

namespace cc {

String String::alignedLeft(int w, char blank) const
{
    return (count() > w) ? *this : *this + String::allocate(w - count(), blank);
}

String String::alignedRight(int w, char blank) const
{
    return (count() > w) ? *this : String::allocate(w - count(), blank) + *this;
}

bool String::find(const char *b, long bn, InOut<long> i0) const
{
    assert(0 <= i0 && i0 <= count());

    if (bn < 0) bn = len(b);

    long i = i0;
    long n = count();
    const char *a = chars();

    if (n <= i || bn == 0) {
        i0 = n;
        return false;
    }

    for (long j = i, k = 0; j < n;) {
        if (a[j++] == b[k]) {
            ++k;
            if (k == bn) {
                i0 = j - k;
                return true;
            }
        }
        else {
            k = 0;
        }
    }

    i0 = n;
    return false;
}

void String::replace(const char *b, const char *s, long bn, long sn)
{
    if (bn < sn) {
        if (find(b)) {
            (*this) = String{split(b, bn), s, sn};
        }
    }
    else if (bn > 0) {
        long i = 0, j = 0, k = 0, n = me().count;
        char *a = chars();
        while (i < n) {
            char ch = a[i++];
            if (j < i) a[j++] = ch;
            if (ch == b[k]) {
                ++k;
                if (k == bn) {
                    j -= bn;
                    for (k = 0; k < sn; ++k)
                        a[j++] = s[k];
                    k = 0;
                }
            }
            else {
                k = 0;
            }
        }
        truncate(j);
    }
}

List<String> String::split(char sep) const
{
    List<String> parts;
    long n = me().count;
    for (long i = 0; i < n;) {
        long j = i;
        find(sep, &j);
        parts << copy(i, j);
        i = j + 1;
        if (i == n) {
            parts << String{};
            break;
        }
    }
    return parts;
}

List<String> String::split(const char *sep, long sepLength) const
{
    List<String> parts;
    long i0 = 0;
    long n = me().count;
    while (i0 < n) {
        long i1 = i0;
        if (!find(sep, &i1)) break;
        parts << copy(i0, i1);
        i0 = i1 + sepLength;
    }
    if (i0 < n)
        parts << copy(i0, n);
    else
        parts << String{};
    return parts;
}

List<String> String::breakUp(long m) const
{
    List<String> parts;
    long i0 = 0;
    long n = me().count;
    while (i0 < n) {
        long i1 = i0 + m;
        if (i1 > n) i1 = n;
        parts << copy(i0, i1);
        i0 += m;
    }
    return parts;
}

String String::times(long n) const
{
    if (me().count == 0) return String{};

    String result = String::allocate(me().count * n);
    long offset = 0;
    for (; n > 0; --n) {
        std::memcpy(result.chars() + offset, chars(), count() * sizeof(char));
        offset += me().count;
    }

    return result;
}

String String::paste(long i0, long i1, const String &text) const
{
    assert(0 <= i0 && i0 <= i1 && i1 <= count());
    if (i0 == i1 && text.count() == 0) return copy();

    String result = String::allocate(i0 + text.count() + count() - i1);

    if (0 < i0) std::memcpy(result.chars(), chars(), i0 * sizeof(char));
    if (0 < text.count()) std::memcpy(result.chars() + i0, text.chars(), text.count() * sizeof(char));
    if (i1 < count()) std::memcpy(result.chars() + i0 + text.count(), chars() + i1, (count() - i1) * sizeof(char));

    return result;
}

void String::trim(const char *ls, const char *ts)
{
    if (!ts) ts = ls;
    long i0 = 0, i1 = count();
    char *a = chars();
    while (i0 < i1) {
        const char *p = ls;
        for (; *p; ++p)
            if (a[i0] == *p) break;
        if (!*p) break;
        ++i0;
    }
    while (i0 < i1) {
        const char *p = ts;
        for (; *p; ++p)
            if (a[i1 - 1] == *p) break;
        if (!*p) break;
        --i1;
    }
    if (i0 > 0 && i0 < i1) std::memmove(a, a + i0, i1 - i0);
    truncate(i1 - i0);
}

void String::simplify(const char *ws)
{
    long j = 0;
    long n = count();
    char *a = chars();
    for (long i = 0, s = 0; i < n; ++i) {
        const char *p = ws;
        for (; *p; ++p)
            if (a[i] == *p) break;
        s = (*p) ? s + 1 : 0;
        a[j] = (*p) ? ' ' : a[i];
        j += (s < 2);
    }
    truncate(j);
    trim(ws);
}

String String::escaped() const
{
    List<String> parts;
    int i = 0, i0 = 0, n = count();
    for (; i < n; ++i) {
        uint8_t ch = item<uint8_t>(i);
        if (ch < 32 || 127 <= ch) {
            if (i0 < i) parts.append(copy(i0, i));
            i0 = i + 1;
            if (ch == 0x08) parts.append("\\b");
            else if (ch == 0x09) parts.append("\\t");
            else if (ch == 0x0A) parts.append("\\n");
            else if (ch == 0x0D) parts.append("\\r");
            else if (ch == 0x0C) parts.append("\\f");
            else {
                String s = "\\xXX";
                const char *hex = "0123456789ABCDEF";
                s.at(s.count() - 2) = hex[ch / 16];
                s.at(s.count() - 1) = hex[ch % 16];
                parts.append(s);
            }
        }
    }
    if (parts.count() == 0) return copy();

    if (i0 < i) parts.append(copy(i0, i));

    return parts;
}

void String::expand()
{
    if (!find('\\')) return;
    int j = 0;
    uint32_t hs = 0; // high surrogate, saved
    String ec; // buffer for encoded character
    for (int i = 0, n = count(); i < n;) {
        char ch = at(i++);
        if ((ch == '\\') && (i < n)) {
            ch = at(i++);
            if ((ch == 'u') && (i <= n - 4)) {
                uint32_t x = 0;
                for (int k = 0; k < 4; ++k) {
                    int digit = at(i++);
                    if (('0' <= digit) && (digit <= '9')) digit -= '0';
                    else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
                    else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
                    x = (x * 16) + digit;
                }
                if ((0xB800 <= x) && (x < 0xDC00)) {
                    // save the high surrogate, do not output anything
                    hs = x;
                }
                else {
                    if ((0xDC00 <= x) && (x < 0xE000) && (hs != 0)) {
                        // combine the high and low surrogate
                        x = ((hs - 0xD800) << 10) | (x - 0xDC00);
                        x += 0x10000;
                        hs = 0;
                    }
                    if (ec.count() == 0) ec = String::allocate(4);
                    Utf8Sink sink{ec};
                    sink.write(x);
                    const int el = sink.currentOffset();
                    for (int k = 0; k < el; ++k)
                        at(j++) = ec.at(k);
                }
            }
            else if ((ch == 'x') && (i <= n - 2)) {
                uint8_t x = 0;
                for (int k = 0; k < 2; ++k) {
                    int digit = at(i++);
                    if (('0' <= digit) && (digit <= '9')) digit -= '0';
                    else if (('a' <= digit) && (digit <= 'f')) digit = digit - 'a' + 10;
                    else if (('A' <= digit) && (digit <= 'F')) digit = digit - 'A' + 10;
                    x = (x * 16) + digit;
                }
                at(j++) = (char)x;
            }
            else {
                hs = 0;
                if (ch == 'b') at(j++) = 0x08;
                else if (ch == 't') at(j++) = 0x09;
                else if (ch == 'n') at(j++) = 0x0A;
                else if (ch == 'r') at(j++) = 0x0D;
                else if (ch == 'f') at(j++) = 0x0C;
                else if (ch == 's') at(j++) = 0x20;
                else at(j++) = ch;
            }
        }
        else if (j < i) {
            hs = 0;
            at(j++) = ch;
        }
        else {
            hs = 0;
            ++j;
        }
    }
    truncate(j);
}

bool String::toBool(Out<bool> ok) const
{
    bool value = false;
    ok = readBool(&value);
    return value;
}

int String::toInt(Out<bool> ok) const
{
    int okCount = 0;
    int value = readNumber<int>(*this, &okCount);
    ok = (okCount == 1);
    return value;
}

long String::toLong(Out<bool> ok) const
{
    int okCount = 0;
    long value = readNumber<long>(*this, &okCount);
    ok = (okCount == 1);
    return value;
}

double String::toDouble(Out<bool> ok) const
{
    int okCount = 0;
    double value = readNumber<double>(*this, &okCount);
    ok = (okCount == 1);
    return value;
}

bool String::readBool(Out<bool> value) const
{
    bool ok = false;

    if (
        Casefree<String>(*this) == "on" ||
        Casefree<String>(*this) == "true" ||
        Casefree<String>(*this) == "1"
    ) {
        value = true;
        ok = true;
    }
    else if (
        Casefree<String>(*this) == "off" ||
        Casefree<String>(*this) == "false" ||
        Casefree<String>(*this) == "0"
    ) {
        value = false;
        ok = true;
    }

    return ok;
}

bool String::isHerePath() const
{
    const char *s = chars();
    const long n = count();
    if (n == 0) return false;
    if (s[0] != '.') return false;
    int m = 0;
    for (long i = 0; i < n; ++i) {
        char ch = s[i];
        if (ch == '.') {
            ++m;
            if (m == 2) return false;
        }
        else if (ch == '/') {
            m = 0;
        }
        else
            return false;
    }
    return true;
}

String String::absolutePathRelativeTo(const String &currentDir) const
{
    if (isAbsolutePath() || (currentDir == "."))
        return copy();

    List<String> absoluteParts;
    List<String> parts = split('/');

    int upCount = 0;

    for (const String &c: parts)
    {
        if (c == ".")
        {}
        else if (c == "..") {
            if (absoluteParts.count() > 0)
                absoluteParts.popBack();
            else
                ++upCount;
        }
        else {
            absoluteParts.append(c);
        }
    }

    String prefix = currentDir.copy();

    while (upCount > 0) {
        prefix = prefix.cdUp();
        --upCount;
    }

    absoluteParts.pushFront(prefix);

    return absoluteParts.join('/');
}

String String::fileName() const
{
    long i = count();
    while (i > 0) {
        --i;
        if (at(i) == '/') {
            ++i;
            break;
        }
    }
    return copy(i, count());
}

String String::baseName() const
{
    String name = fileName();
    long i = 0;
    if (name.find('.', &i)) name.truncate(i);
    return name;
}

String String::fileSuffix() const
{
    String name = fileName();
    long i = 0;
    return name.find('.', &i) ? name.copy(i + 1, name.count()) : String{};
}

String String::operator/(const String &relativePath) const
{
    if (count() == 0/* || isHerePath()*/) return relativePath.copy();
    if (relativePath.count() == 0) return copy();

    List<String> parts;
    if (!isRootPath()) parts << *this;
    if (!endsWith('/') || !relativePath.startsWith('/')) parts << "/";
    parts << relativePath;

    return parts;
}

String String::cd(const String &target) const
{
    if (target == "..") return cdUp();
    if (target == ".") return copy();

    if ((*this) == "" || (*this) == "." || target.isAbsolutePath())
        return target.copy();

    List<String> absoluteParts;
    List<String> parts = target.split('/');

    int upCount = 0;

    for (const String &c: parts)
    {
        if (c == ".")
        {}
        else if (c == "..") {
            if (absoluteParts.count() > 0)
                absoluteParts.popBack();
            else
                ++upCount;
        }
        else {
            absoluteParts.append(c);
        }
    }

    String prefix = *this;
    while (upCount > 0) {
        prefix = prefix.cdUp();
        --upCount;
    }

    absoluteParts.pushFront(prefix);

    return String{absoluteParts, '/'};
}

String String::cdUp() const
{
    List<String> parts = split('/');
    while (parts.count() > 0) {
        String component = parts.last();
        parts.popBack();
        if (component != "") break;
    }

    String resultPath{parts, '/'};
    if (resultPath == "")
        resultPath = isAbsolutePath() ? "/" : ".";

    return resultPath;
}

String String::canonicalPath() const
{
    if (!(
        endsWith('/') || find("//") || find("./") || endsWith("/.")
    )) return copy();

    List<String> parts = split('/');
    List<String> result;
    for (Locator pos = parts.head(); pos; ++pos) {
        String part = parts.at(pos);
        if (part == "" && +pos > 0) continue;
        if (part == "" && +pos == parts.count() - 1) continue;
        if (part == "." && parts.count() > 1) continue;
        if (part == ".." && result.count() > 0) {
            if (result.last() != "..") {
                result.popBack();
                continue;
            }
        }
        result.append(part);
    }
    return String{result, '/'};
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Color>
#include <cc/Map>
#include <cc/Casefree>
#include <cc/str>
#include <limits>
#include <cmath>

namespace cc {

const Color Color::Transparent { 0x00, 0x00, 0x00, 0x00 };
const Color Color::Black       { 0x00, 0x00, 0x00 };
const Color Color::White       { 0xFF, 0xFF, 0xFF };
const Color Color::Red         { 0xFF, 0x00, 0x00 };
const Color Color::Green       { 0x00, 0xFF, 0x00 };
const Color Color::Blue        { 0x00, 0x00, 0xFF };

Color Color::fromHsv(double h, double s, double v)
{
    double r, g, b;
    hsvToRgb(h, s, v, &r, &g, &b);
    return Color{
        uint32_t(round(0xFF * r)),
        uint32_t(round(0xFF * g)),
        uint32_t(round(0xFF * b))
    };
}

Color Color::fromHsl(double h, double s, double l)
{
    double r, g, b;
    hslToRgb(h, s, l, &r, &g, &b);
    return Color{
        uint32_t(round(0xFF * r)),
        uint32_t(round(0xFF * g)),
        uint32_t(round(0xFF * b))
    };
}

void Color::applyOver(Color b)
{
    const uint32_t h = component<AlphaShift, uint32_t>();

    if (h == 0xFF)
    {
        uint16_t o_b = b.component<AlphaShift, uint16_t>();
        if (o_b == 0xFF) {
            *this = b;
            return;
        }

        uint16_t n_b = 0x100 - o_b;

        typedef uint16_t v4ui16 __attribute__((vector_size(8)));

        v4ui16 v_b {
            b.component<RedShift  , uint16_t>(),
            b.component<GreenShift, uint16_t>(),
            b.component<BlueShift , uint16_t>(),
            0
        };

        v4ui16 v_a {
            component<RedShift,   uint16_t>(),
            component<GreenShift, uint16_t>(),
            component<BlueShift,  uint16_t>(),
            0
        };

        v_a = (o_b * v_b + n_b * v_a) >> 8;

        me = compose(v_a[0], v_a[1], v_a[2]);
    }
    else {
        uint32_t o_b = b.component<AlphaShift, uint32_t>();
        if (o_b == 0xFF) {
            *this = b;
            return;
        }

        uint32_t n_b = 0xFF - o_b;
        uint32_t o_a = h;

        o_b <<= 8;
        n_b *= o_a;
        uint32_t h = o_b + n_b;

        if ((h >> 8) > 0)
        {
            typedef uint32_t v4ui32 __attribute__((vector_size(16)));

            v4ui32 v_b {
                b.component<RedShift,   uint32_t>(),
                b.component<GreenShift, uint32_t>(),
                b.component<BlueShift,  uint32_t>(),
                0
            };

            v4ui32 v_a {
                component<RedShift,   uint32_t>(),
                component<GreenShift, uint32_t>(),
                component<BlueShift,  uint32_t>(),
                0
            };

            v_a = (o_b * v_b + n_b * v_a) / h;

            me = compose(v_a[0], v_a[1], v_a[2], h >> 8);
        }
        else {
            me = 0;
        }
    }
}

void Color::mixIn(Color b, int percent)
{
    typedef uint32_t v4ui32 __attribute__((vector_size(16)));

    v4ui32 v_b {
        b.component<RedShift,   uint32_t>(),
        b.component<GreenShift, uint32_t>(),
        b.component<BlueShift,  uint32_t>(),
        b.component<AlphaShift, uint32_t>()
    };

    v4ui32 v_a {
        component<RedShift,   uint32_t>(),
        component<GreenShift, uint32_t>(),
        component<BlueShift,  uint32_t>(),
        component<AlphaShift, uint32_t>()
    };

    v_a = (v_a * (100 - percent) + v_b * percent) / 100;

    me = compose(v_a[0], v_a[1], v_a[2], v_a[3]);
}

String Color::toString() const
{
    List<String> parts;
    parts << "#" << hex(red(), 2) << hex(green(), 2) << hex(blue(), 2);
    if (alpha() != 0xFF) parts << hex(alpha(), 2);
    return parts;
}

Color Color::parse(const char *s)
{
    if (!s[0]) return Color{};

    if (s[0] == '#') {
        struct H {
            static inline int dehex(char ch, Out<bool> ok) {
                if ('0' <= ch && ch <= '9') return ch - '0';
                else if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
                else if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
                ok = false;
                return -1;
            }
        };
        Color c;
        int n = 1;
        while (s[n] && n < 11) ++n;
        if (n == 7) { // #RRGGBB
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int rl = H::dehex(s[2], &ok);
            int gh = H::dehex(s[3], &ok);
            int gl = H::dehex(s[4], &ok);
            int bh = H::dehex(s[5], &ok);
            int bl = H::dehex(s[6], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rl,
                (gh << 4) | gl,
                (bh << 4) | bl
            };
        }
        else if (n == 9) { // #RRGGBBAA
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int rl = H::dehex(s[2], &ok);
            int gh = H::dehex(s[3], &ok);
            int gl = H::dehex(s[4], &ok);
            int bh = H::dehex(s[5], &ok);
            int bl = H::dehex(s[6], &ok);
            int ah = H::dehex(s[7], &ok);
            int al = H::dehex(s[8], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rl,
                (gh << 4) | gl,
                (bh << 4) | bl,
                (ah << 4) | al
            };
        }
        else if (n == 4) { // #RGB
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int gh = H::dehex(s[2], &ok);
            int bh = H::dehex(s[3], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rh,
                (gh << 4) | gh,
                (bh << 4) | bh
            };
        }
        else if (n == 5) { // #RGBA
            bool ok = true;
            int rh = H::dehex(s[1], &ok);
            int gh = H::dehex(s[2], &ok);
            int bh = H::dehex(s[3], &ok);
            int ah = H::dehex(s[4], &ok);
            if (!ok) return Color{};
            return Color{
                (rh << 4) | rh,
                (gh << 4) | gh,
                (bh << 4) | bh,
                (ah << 4) | ah
            };
        }
        return Color{};
    }
    Color c;
    lookup(s, &c);
    return c;
}

bool Color::lookup(const String &name, Out<Color> color)
{
    static thread_local Map<Casefree<String>, Color> svgPalette {
        {"aliceblue"           , { 240, 248, 255 }},
        {"antiquewhite"        , { 250, 235, 215 }},
        {"aqua"                , {   0, 255, 255 }},
        {"aquamarine"          , { 127, 255, 212 }},
        {"azure"               , { 240, 255, 255 }},
        {"beige"               , { 245, 245, 220 }},
        {"bisque"              , { 255, 228, 196 }},
        {"black"               , {   0,   0,   0 }},
        {"blanchedalmond"      , { 255, 235, 205 }},
        {"blue"                , {   0,   0, 255 }},
        {"blueviolet"          , { 138,  43, 226 }},
        {"brown"               , { 165,  42,  42 }},
        {"burlywood"           , { 222, 184, 135 }},
        {"cadetblue"           , {  95, 158, 160 }},
        {"chartreuse"          , { 127, 255,   0 }},
        {"chocolate"           , { 210, 105,  30 }},
        {"coral"               , { 255, 127,  80 }},
        {"cornflowerblue"      , { 100, 149, 237 }},
        {"cornsilk"            , { 255, 248, 220 }},
        {"crimson"             , { 220,  20,  60 }},
        {"cyan"                , {   0, 255, 255 }},
        {"darkblue"            , {   0,   0, 139 }},
        {"darkcyan"            , {   0, 139, 139 }},
        {"darkgoldenrod"       , { 184, 134,  11 }},
        {"darkgray"            , { 169, 169, 169 }},
        {"darkgreen"           , {   0, 100,   0 }},
        {"darkgrey"            , { 169, 169, 169 }},
        {"darkkhaki"           , { 189, 183, 107 }},
        {"darkmagenta"         , { 139,   0, 139 }},
        {"darkolivegreen"      , {  85, 107,  47 }},
        {"darkorange"          , { 255, 140,   0 }},
        {"darkorchid"          , { 153,  50, 204 }},
        {"darkred"             , { 139,   0,   0 }},
        {"darksalmon"          , { 233, 150, 122 }},
        {"darkseagreen"        , { 143, 188, 143 }},
        {"darkslateblue"       , {  72,  61, 139 }},
        {"darkslategray"       , {  47,  79,  79 }},
        {"darkslategrey"       , {  47,  79,  79 }},
        {"darkturquoise"       , {   0, 206, 209 }},
        {"darkviolet"          , { 148,   0, 211 }},
        {"deeppink"            , { 255,  20, 147 }},
        {"deepskyblue"         , {   0, 191, 255 }},
        {"dimgray"             , { 105, 105, 105 }},
        {"dimgrey"             , { 105, 105, 105 }},
        {"dodgerblue"          , {  30, 144, 255 }},
        {"firebrick"           , { 178,  34,  34 }},
        {"floralwhite"         , { 255, 250, 240 }},
        {"forestgreen"         , {  34, 139,  34 }},
        {"fuchsia"             , { 255,   0, 255 }},
        {"gainsboro"           , { 220, 220, 220 }},
        {"ghostwhite"          , { 248, 248, 255 }},
        {"gold"                , { 255, 215,   0 }},
        {"goldenrod"           , { 218, 165,  32 }},
        {"gray"                , { 128, 128, 128 }},
        {"grey"                , { 128, 128, 128 }},
        {"green"               , {   0, 128,   0 }},
        {"greenyellow"         , { 173, 255,  47 }},
        {"honeydew"            , { 240, 255, 240 }},
        {"hotpink"             , { 255, 105, 180 }},
        {"indianred"           , { 205,  92,  92 }},
        {"indigo"              , {  75,   0, 130 }},
        {"ivory"               , { 255, 255, 240 }},
        {"khaki"               , { 240, 230, 140 }},
        {"lavender"            , { 230, 230, 250 }},
        {"lavenderblush"       , { 255, 240, 245 }},
        {"lawngreen"           , { 124, 252,   0 }},
        {"lemonchiffon"        , { 255, 250, 205 }},
        {"lightblue"           , { 173, 216, 230 }},
        {"lightcoral"          , { 240, 128, 128 }},
        {"lightcyan"           , { 224, 255, 255 }},
        {"lightgoldenrodyellow", { 250, 250, 210 }},
        {"lightgray"           , { 211, 211, 211 }},
        {"lightgreen"          , { 144, 238, 144 }},
        {"lightgrey"           , { 211, 211, 211 }},
        {"lightpink"           , { 255, 182, 193 }},
        {"lightsalmon"         , { 255, 160, 122 }},
        {"lightseagreen"       , {  32, 178, 170 }},
        {"lightskyblue"        , { 135, 206, 250 }},
        {"lightslategray"      , { 119, 136, 153 }},
        {"lightslategrey"      , { 119, 136, 153 }},
        {"lightsteelblue"      , { 176, 196, 222 }},
        {"lightyellow"         , { 255, 255, 224 }},
        {"lime"                , {   0, 255,   0 }},
        {"limegreen"           , {  50, 205,  50 }},
        {"linen"               , { 250, 240, 230 }},
        {"magenta"             , { 255,   0, 255 }},
        {"maroon"              , { 128,   0,   0 }},
        {"mediumaquamarine"    , { 102, 205, 170 }},
        {"mediumblue"          , {   0,   0, 205 }},
        {"mediumorchid"        , { 186,  85, 211 }},
        {"mediumpurple"        , { 147, 112, 219 }},
        {"mediumseagreen"      , {  60, 179, 113 }},
        {"mediumslateblue"     , { 123, 104, 238 }},
        {"mediumspringgreen"   , {   0, 250, 154 }},
        {"mediumturquoise"     , {  72, 209, 204 }},
        {"mediumvioletred"     , { 199,  21, 133 }},
        {"midnightblue"        , {  25,  25, 112 }},
        {"mintcream"           , { 245, 255, 250 }},
        {"mistyrose"           , { 255, 228, 225 }},
        {"moccasin"            , { 255, 228, 181 }},
        {"navajowhite"         , { 255, 222, 173 }},
        {"navy"                , {   0,   0, 128 }},
        {"oldlace"             , { 253, 245, 230 }},
        {"olive"               , { 128, 128,   0 }},
        {"olivedrab"           , { 107, 142,  35 }},
        {"orange"              , { 255, 165,   0 }},
        {"orangered"           , { 255,  69,   0 }},
        {"orchid"              , { 218, 112, 214 }},
        {"palegoldenrod"       , { 238, 232, 170 }},
        {"palegreen"           , { 152, 251, 152 }},
        {"paleturquoise"       , { 175, 238, 238 }},
        {"palevioletred"       , { 219, 112, 147 }},
        {"papayawhip"          , { 255, 239, 213 }},
        {"peachpuff"           , { 255, 218, 185 }},
        {"peru"                , { 205, 133,  63 }},
        {"pink"                , { 255, 192, 203 }},
        {"plum"                , { 221, 160, 221 }},
        {"powderblue"          , { 176, 224, 230 }},
        {"purple"              , { 128,   0, 128 }},
        {"red"                 , { 255,   0,   0 }},
        {"rosybrown"           , { 188, 143, 143 }},
        {"royalblue"           , {  65, 105, 225 }},
        {"saddlebrown"         , { 139,  69,  19 }},
        {"salmon"              , { 250, 128, 114 }},
        {"sandybrown"          , { 244, 164,  96 }},
        {"seagreen"            , {  46, 139,  87 }},
        {"seashell"            , { 255, 245, 238 }},
        {"sienna"              , { 160,  82,  45 }},
        {"silver"              , { 192, 192, 192 }},
        {"skyblue"             , { 135, 206, 235 }},
        {"slateblue"           , { 106,  90, 205 }},
        {"slategray"           , { 112, 128, 144 }},
        {"slategrey"           , { 112, 128, 144 }},
        {"snow"                , { 255, 250, 250 }},
        {"springgreen"         , {   0, 255, 127 }},
        {"steelblue"           , {  70, 130, 180 }},
        {"tan"                 , { 210, 180, 140 }},
        {"teal"                , {   0, 128, 128 }},
        {"thistle"             , { 216, 191, 216 }},
        {"tomato"              , { 255,  99,  71 }},
        {"turquoise"           , {  64, 224, 208 }},
        {"violet"              , { 238, 130, 238 }},
        {"wheat"               , { 245, 222, 179 }},
        {"white"               , { 255, 255, 255 }},
        {"whitesmoke"          , { 245, 245, 245 }},
        {"yellow"              , { 255, 255,   0 }},
        {"yellowgreen"         , { 154, 205,  50 }}
    };

    return svgPalette.lookup(name, &color);
}

template<class T>
T min(T a, T b, T c)
{
    T min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

template<class T>
T max(T a, T b, T c)
{
    T max = a;
    if (max < b) max = b;
    if (max < c) max = c;
    return max;
}

void rgbToHsv(double r, double g, double b, Out<double> h, Out<double> s, Out<double> v)
{
    double min = cc::min(r, g, b);
    double max = cc::max(r, g, b);

    v = max;
    s = (max != 0.) ? ((max - min) / max) : 0.;
    if (s == 0.) {
        h = 0.;
        return;
    }

    double delta = max - min;
    if (r == max)
        h = (g - b) / delta;
    else if (g == max)
        h = 2. + (b - r) / delta;
    else /*if (b == max) */
        h = 4. + (r - g) / delta;
    h = h * 60.;
    if (h < 0.) h = h + 360.;
}

void hsvToRgb(double h, double s, double v, Out<double> r, Out<double> g, Out<double> b)
{
    if (s == 0.) {
        r = v;
        g = v;
        b = v;
        return;
    }

    double f, p, q, t;
    int i;

    if (h == 360.) h = 0.;
    h /= 60.;
    i = floor(h);
    f = h - i;
    p = v * (1. - s);
    q = v * (1. - s * f);
    t = v * (1. - s * (1. - f));

    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    };
}

void rgbToHsl(double r, double g, double b, Out<double> h, Out<double> s, Out<double> l)
{
    double min = cc::min(r, g, b);
    double max = cc::max(r, g, b);

    l = (min + max) / 2.;
    if (min == max) {
        s = 0.;
        h = 0.;
        return;
    }

    double delta = max - min;
    s = (l <= 0.5) ? (delta / (min + max)) : (delta / (2. - (min + max)));
    if (r == max)
        h = (g - b) / delta;
    else if (g == max)
        h = 2. + (b - r) / delta;
    else if (b == max)
        h = 4. + (r - g) / delta;
    h = h * 60.;
    if (h < 0.) h = h + 360.;
}

void hslToRgb(double h, double s, double l, Out<double> r, Out<double> g, Out<double> b)
{
    double c = (1. - abs(2. * l - 1.)) * s;
    double q = h / 60.;
    double x = c * (1. - abs(fmod(q, 2.) - 1.));

    double r1, g1, b1;
    if (q <= 1)      { r1 = c; g1 = x; b1 = 0; }
    else if (q <= 2) { r1 = x; g1 = c; b1 = 0; }
    else if (q <= 3) { r1 = 0; g1 = c; b1 = x; }
    else if (q <= 4) { r1 = 0; g1 = x; b1 = c; }
    else if (q <= 5) { r1 = x; g1 = 0; b1 = c; }
    else             { r1 = c; g1 = 0; b1 = x; }

    double m = l - c / 2.;
    r = r1 + m;
    g = g1 + m;
    b = b1 + m;
}

const TypeInfo VariantType<Color>::info
{
    .typeName = "Color",
    .str = [](const void *bytes) {
        return VariantType<Color>::retrieve(bytes).toString();
    },
    .cleanup = [](void *bytes){},
    .assign = [](void *dst, const void *src) {
        new(dst)Color{*static_cast<const Color *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<Color>::retrieve(a) == VariantType<Color>::retrieve(b);
    }
};

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourceContext>
#include <cc/Queue>

namespace cc {

struct ResourceContext::State: public Object::State
{
    Queue<String> queue_;
};

ResourceContext::ResourceContext()
{
    initOncePerThread<State>();
}

void ResourceContext::push(const String &resource)
{
    me().queue_.pushBack(resource);
}

String ResourceContext::pop()
{
    if (me().queue_.count() == 0) return String{};
    String resource = me().queue_.last();
    me().queue_.popBack();
    return resource;
}

String ResourceContext::top() const
{
    return me().queue_.count() > 0 ? me().queue_.last() : String{};
}

ResourceContext::State &ResourceContext::me()
{
    return Object::me.as<State>();
}

const ResourceContext::State &ResourceContext::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/bucket/Tree>

namespace cc {
namespace bucket {

Node *Tree::stepDownTo(long index, unsigned *egress) const
{
    assert(0 <= index && index <= root_.weight_);

    Node *node = root_.node_;

    if (index + 1 >= root_.weight_) {
        node = lastLeaf_;
        *egress = (node) ? node->fill_ - (index < root_.weight_) : 0;
    }
    else if (index <= 0) {
        while (node->isBranch_) node = static_cast<const Branch *>(node)->at(0).node_;
        *egress = 0;
    }
    else if (isDense_ && Capacity == 16) {
        *egress = index & 0xFu;
        for (unsigned h = height_; h > 0; --h) {
            node = static_cast<const Branch *>(node)->at((index >> (h << 2)) & 0xFu).node_;
        }
    }
    else {
        while (node->isBranch_) {
            const Branch *branch = static_cast<const Branch *>(node);
            const unsigned n = branch->fill_;
            for (unsigned i = 0; i < n; ++i) {
                const Head *head = &branch->at(i);
                index -= head->weight_;
                if (index < 0) {
                    node = head->node_;
                    index += head->weight_;
                    break;
                }
            }
        }
        *egress = index;
    }

    return node;
}

void Tree::joinSucc(Node *node, Node *newNode)
{
    Node *oldSucc = node->succ();

    if (oldSucc) {
        newNode->succ_ = oldSucc;
        oldSucc->pred_ = newNode;
        isDense_ = false;
    }
    else if (!newNode->isBranch_) {
        lastLeaf_ = newNode;
    }

    newNode->pred_ = node;
    node->succ_ = newNode;

    Branch *parent = node->parent_;
    if (parent) {
        unsigned newNodeIndex = node->parent_->indexOf(node) + 1;
        dissipate(parent, newNodeIndex);
        parent->push(newNodeIndex, Head{.weight_ = 0, .node_ = newNode});
    }
    else {
        Branch *branch = new Branch;
        branch->push(0, root_);
        branch->push(1, Head{.weight_ = 0, .node_ = newNode});
        root_.node_ = branch;
        ++height_;
    }
}

void Tree::unlink(Node *node)
{
    Branch *parent = node->parent_;
    parent->pop(parent->indexOf(node));

    Node *succ = node->succ();
    Node *pred = node->pred();
    if (pred) pred->succ_ = succ;
    if (succ) succ->pred_ = pred;
    else if (!node->isBranch_) lastLeaf_ = pred;

    delete node;

    relieve(parent);
}

void Tree::shiftWeights(Node *from, Node *to, long delta)
{
    while (from != to) {
        weight(from) -= delta;
        weight(to) += delta;
        from = from->parent_;
        to = to->parent_;
    }
}

void Tree::updateWeights(Node *node, long delta)
{
    while (node != root_.node_) {
        weight(node) += delta;
        node = node->parent_;
    }
    root_.weight_ += delta;
}

void Tree::reduce()
{
    if (root_.node_) {
        while (root_.node_->isBranch_ && root_.node_->fill_ == 1) {
            Branch *branch = static_cast<Branch *>(root_.node_);
            root_.node_ = branch->at(0).node_;
            delete branch;
            --height_;
        }
        root_.node_->parent_ = nullptr;
    }
}

template void Tree::dissipate<Branch>(Branch *&node, unsigned &egress);
template void Tree::relieve<Branch>(Branch *node, bool recursive);
template void Tree::dissipateForward<Branch>(Branch *from, Branch *to);
template void Tree::dissipateBackward<Branch>(Branch *to, Branch *from);
template void Tree::distributeForward<Branch>(Branch *from, Branch *to);
template void Tree::collapseSucc<Branch>(Branch *node, Branch *succ);

}} // namespace cc::bucket
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LineSource>

namespace cc {

struct LineSource::State: public Object::State
{
    State(const Stream &stream, const String &buffer):
        stream_{stream},
        buffer_{buffer},
        eoi_{false},
        bufferOffset_{0},
        i0_{0},
        i_{0},
        n_{(stream) ? 0 : buffer.count()}
    {}

    bool read(Out<String> line)
    {
        if (eoi_) {
            line << String{};
            return false;
        }

        List<String> backlog;

        while (true) {
            if (i_ < n_) {
                i0_ = i_;
                i_ = findEol(buffer_, n_, i_);
                if (i_ < n_) {
                    if (backlog.count() > 0) {
                        backlog.append(buffer_.copy(i0_, i_));
                        line << backlog;
                    }
                    else {
                        line << buffer_.copy(i0_, i_);
                    }
                    i_ = skipEol(buffer_, n_, i_);
                    return true;
                }
                backlog.append(buffer_.copy(i0_, i_));
            }

            if (!stream_) break;

            bufferOffset_ += n_;
            n_ = stream_.read(&buffer_);
            if (n_ == 0) break;
            i_ = 0;
        }

        eoi_ = true;
        if (backlog.count() > 0) {
            line << backlog;
            return true;
        }
        line << String{};
        return false;
    }

    static long findEol(const Bytes &buffer, long n, long i)
    {
        for (; i < n; ++i) {
            char ch = buffer.at(i);
            if (ch == '\n' || ch == '\r' || ch == '\0')
                break;
        }
        return i;
    }

    static long skipEol(const Bytes &buffer, long n, long i)
    {
        if (i < n) if (buffer.at(i) == '\r') ++i;
        if (i < n) if (buffer.at(i) == '\n') ++i;
        if (i < n) if (buffer.at(i) == '\0') ++i;
        return i;
    }

    Stream stream_;
    String buffer_;
    bool eoi_;
    long bufferOffset_;
    long i0_, i_, n_;
};

LineSource::LineSource(const String &buffer):
    Object{new State{Stream{}, buffer}}
{}

LineSource::LineSource(const Stream &stream):
    Object{new State{stream, String::allocate(0x1000)}}
{}

LineSource::LineSource(const Stream &stream, const String &buffer):
    Object{new State{stream, buffer}}
{}

bool LineSource::read(Out<String> line)
{
    return me().read(line);
}

LineSource::State &LineSource::me()
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Bundle>
#include <cc/Process>
#include <cc/File>

namespace cc {

Bundle &Bundle::instance()
{
    static Bundle instance_;
    return instance_;
}

String Bundle::lookup(const String &relPath, const List<String> &dirs)
{
    return File::locate(relPath, dirs, FileAccess::Exists);
}

String Bundle::prefix(const char *defaultPrefix)
{
    Bundle &bundle = instance();
    if (bundle.overridePrefix_ != "")
        return bundle.overridePrefix_;
    return defaultPrefix;
}

String Bundle::exePrefix()
{
    return Bundle::instance().exePrefix_;
}

Bundle::Bundle():
    overridePrefix_{Process::env("CC_BUNDLE_PREFIX_OVERRIDE")},
    exePrefix_{Process::execPath().cdUp()}
{}

} // namespace cc
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Resource>
#include <cc/ResourceManager>

namespace cc {

ResourceRegistration::ResourceRegistration(const String &realPrefix)
{
    ResourceManager{}.addDirectory(realPrefix);
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TextError>
#include <cc/ResourceContext>
#include <cc/Format>
#include <cc/input>

namespace cc {

TextError::TextError(const String &text, long offset, const String &hint):
    source_{ResourceContext{}.top()},
    text_{text},
    offset_{offset},
    hint_{hint}
{}

String TextError::message() const
{
    if (text_.contains('\n')) {
        long line = 0, pos = 0;
        if (offsetToLinePos(text_, offset_, &line, &pos)) {
            return (Format{"%%:%%:%%: %%"} << source_ << line << pos << hint_).join<String>();
        }
    }
    return Format{"%%...: %%"} << text_.copy(0, offset_) << hint_;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PropertyBinding>

namespace cc {

class PropertyActivator
{
public:
    PropertyActivator(PropertyBinding *binding):
        parent_{head_},
        binding_{binding}
    {
        if (binding_) {
            ++binding_->cascadeDepth_;
            if (binding_->cascadeDepth_ > 3) throw PropertyBindingLoop{};
        }
        head_ = this;
    }

    ~PropertyActivator()
    {
        if (binding_) --binding_->cascadeDepth_;
        head_ = parent_;
    }

    static PropertyBinding *activeBinding() { return head_ ? head_->binding_ : nullptr; }

    PropertyActivator *operator->() { return this; }

private:
    static thread_local PropertyActivator *head_;
    PropertyActivator *parent_ { nullptr };
    PropertyBinding *binding_ { nullptr };
};

thread_local PropertyActivator *PropertyActivator::head_ { nullptr };

PropertyBinding::PropertyBinding(void *owner, bool dirty):
    owner_{owner},
    dirty_{dirty}
{}

void PropertyBinding::preAccess() const
{
    if (dirty_) const_cast<PropertyBinding *>(this)->cascade();

    PropertyBinding *activeBinding = PropertyActivator::activeBinding();
    if (activeBinding && (activeBinding != this)) {
        if (activeBinding->dependencies_.insert(Handle<PropertyBinding>{this, Alias{}}))
            subscribers_.insert(Handle<PropertyBinding>{activeBinding, Alias{}});
    }
}

bool PropertyBinding::hasConsumers() const
{
    return subscribers_.count() > 0 || changed.hasListeners();
}

void PropertyBinding::clearDependencies()
{
    for (auto &other: dependencies_)
        other().subscribers_.remove(this);
    dependencies_.deplete();
}

void PropertyBinding::clearSubscribers()
{
    for (auto &other: subscribers_)
        other().dependencies_.remove(this);
    subscribers_.deplete();
}

void PropertyBinding::emit()
{
    Association others = subscribers_;
    for (auto &other: others) {
        if (other().hasConsumers())
            other.mutate().cascade();
        else
            other.mutate().dirty_ = true;
    }

    PropertyActivator activator{nullptr};
    changed.emit();
}

void PropertyBinding::cascade()
{
    PropertyActivator activator{this};
    evaluate();
}

void PropertyBinding::disband()
{
    changed.disband();
    clearDependencies();
    while (subscribers_.count() > 0)
        subscribers_.at(0).mutate().disband();
    subscribers_.deplete();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Stream>
#include <cc/NullStream>
#include <sys/uio.h> // iovec
#include <cassert>

namespace cc {

void Stream::State::write(const List<Bytes> &buffers)
{
    for (const auto &b: buffers)
        write(b, b.count());
}

void Stream::State::write(const struct iovec *iov, int iovcnt)
{
    long total = 0;
    for (int i = 0; i < iovcnt; ++i) {
        total += iov[i].iov_len;
    }

    Bytes buffer = Bytes::allocate(total);
    long offset = 0;
    for (int i = 0; i < iovcnt; ++i) {
        std::memcpy(buffer.items() + offset, iov[i].iov_base, iov[i].iov_len);
        offset += iov[i].iov_len;
    }

    write(buffer, total);
}

long long Stream::State::transferTo(const Stream &sink, long long count, const Bytes &buffer)
{
    Bytes buffer_{buffer};
    Stream sink_{sink};
    long long total = 0;

    while (count != 0) {
        long m = (count < 0 || buffer_.count() < count) ? buffer_.count() : count;
        long n = read(&buffer_, m);
        if (n == 0) break;
        sink_.write(buffer_, n);
        total += n;
        count -= n;
    }

    return total;
}

long long Stream::State::transferTo(const Stream &sink, long long count)
{
    const long n = defaultTransferUnit();
    Bytes buffer = Bytes::allocate((0 < count && count < n) ? count : n);
    return transferTo(sink, count, buffer);
}

long long Stream::State::skip(long long count)
{
    NullStream sink;
    return transferTo(sink, count);
}

void Stream::State::drain(const Bytes &auxBuffer)
{
    Bytes buffer = auxBuffer;
    if (!buffer) buffer = Bytes::allocate(defaultTransferUnit());
    NullStream sink;
    transferTo(sink, -1, buffer);
}

long Stream::State::readSpan(Out<Bytes> buffer)
{
    const long w = buffer().count();
    long m = 0;
    while (m < w) {
        auto s = buffer().select(m, w);
        long n = read(&s);
        if (n == 0) break;
        m += n;
    }
    return m;
}

String Stream::State::readSpan(long count)
{
    if (count == 0) return String{};
    if (count < 0) return readAll();
    String s = String::allocate(count, '\0');
    readSpan(&s);
    return s;
}

String Stream::State::readAll(const Bytes &auxBuffer)
{
    Bytes buffer = auxBuffer;
    if (!buffer) buffer = Bytes::allocate(defaultTransferUnit());
    List<Bytes> parts;
    while (true) {
        long n = read(&buffer);
        if (n == 0) break;
        if (n < buffer.count()) {
            parts.append(buffer.copy(0, n));
        }
        else {
            parts.append(buffer);
            buffer = Bytes::allocate(buffer.count());
        }
    }
    return parts;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Crc32Sink>

namespace cc {

static const std::uint32_t crcTable[] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

struct Crc32Sink::State: public HashSink::State
{
    State(std::uint32_t seed = Crc32Sink::DefaultSeed):
        crc_{seed}
    {}

    void feed(const void *buffer, long fill)
    {
        for (int i = 0; i < fill; ++i)
            crc_ = crcTable[(crc_ ^ static_cast<const std::uint8_t *>(buffer)[i]) & 0xFF] ^ (crc_ >> 8);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        return 0;
    }

    void write(const Bytes &buffer, long fill) override
    {
        feed(buffer, (0 < fill) ? fill : +buffer);
    }

    Bytes finish() override
    {
        Bytes sum = Bytes::allocate(HashSize);
        sum[0] = (crc_ >> 24) & 0xFF;
        sum[1] = (crc_ >> 16) & 0xFF;
        sum[2] = (crc_ >> 8) & 0xFF;
        sum[3] = crc_ & 0xFF;
        return sum;
    }

    std::uint32_t crc_;
};

Crc32Sink::Crc32Sink(std::uint32_t seed):
    HashSink{new State{seed}}
{}

std::uint32_t Crc32Sink::sum() const
{
    return me().crc_;
}

const Crc32Sink::State &Crc32Sink::me() const
{
    return Object::me.as<State>();
}

std::uint32_t crc32(const void *buffer, long size)
{
    Crc32Sink::State state;
    if (buffer) state.feed(buffer, size);
    return state.crc_;
}

std::uint32_t crc32(const char *buffer)
{
    Crc32Sink::State state;
    if (buffer) state.feed(buffer, std::strlen(buffer));
    return state.crc_;
}

std::uint32_t crc32(const Bytes &buffer)
{
    Crc32Sink::State state;
    state.feed(buffer, +buffer);
    return state.crc_;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/NullStream>

namespace cc {

struct NullStream::State: public Stream::State
{
    long read(Out<Bytes> buffer, long maxFill) override
    {
        buffer().fill(0);
        return buffer().count();
    }

    bool isDiscarding() const override
    {
        return true;
    }
};

NullStream::NullStream():
    Stream{new State}
{}

} // namespace cc
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PointerIntegrity>
#include <cc/SpinLock>
#include <cc/Map>
#include <cc/Set>

namespace cc {

struct PointerIntegrity::State
{
    using PointerSetByTarget = Map<void *, Set<void **> >;

    static State *instance()
    {
        static State *instance = new State;
            /** Bleeding here on purpose to keep service available until the very end of program.
              */
        return instance;
    }

    void addPointer(void *target, void **pointer)
    {
        Guard<SpinLock> guard{lock_};
        Locator locator;
        if (!pointerSetByTarget_.find(target, &locator)) {
            pointerSetByTarget_.insert(target, Set<void **>{}, &locator);
        }
        pointerSetByTarget_.at(locator).value().insert(pointer);
    }

    void dropPointer(void *target, void **pointer)
    {
        Guard<SpinLock> guard{lock_};
        Locator locator;
        if (pointerSetByTarget_.find(target, &locator)) {
            Set<void **> &set = pointerSetByTarget_.at(locator).value();
            if (set.count() == 1)
                pointerSetByTarget_.removeAt(locator);
            else
                set.remove(pointer);
        }
    }

    void endOfLife(void *target)
    {
        Guard<SpinLock> guard{lock_};
        Locator locator;
        if (pointerSetByTarget_.find(target, &locator)) {
            const Set<void **> &set = pointerSetByTarget_.at(locator).value();
            for (void **p: set) {
                *p = nullptr;
            }
            pointerSetByTarget_.removeAt(locator);
        }
    }

    SpinLock lock_;
    PointerSetByTarget pointerSetByTarget_;
};

void PointerIntegrity::addPointer(void *&pointer)
{
    me().addPointer(pointer, &pointer);
}

void PointerIntegrity::dropPointer(void *&pointer)
{
    me().dropPointer(pointer, &pointer);
}

void PointerIntegrity::endOfLife(void *target)
{
    me().endOfLife(target);
}

PointerIntegrity::State &PointerIntegrity::me()
{
    thread_local State *localInstance = State::instance();
    return *localInstance;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/File>
#include <cc/FileStatus>
#include <cc/Dir>
#include <cc/Random>
#include <cc/str>

namespace cc {

long long File::State::transferTo(const Stream &sink, long long count, const Bytes &buffer)
{
    if (!sink && count > 0) {
        off_t ret = 0;
        if (count > 0) ret = ::lseek(fd_, count, SEEK_CUR);
        else ret = ::lseek(fd_, 0, SEEK_END);
        if (ret != -1) return count;
    }
    //! \todo make use of memory mapped I/O if applicable
    return IoStream::State::transferTo(sink, count, buffer);
}

bool File::access(const String &path, FileAccess mode)
{
    return (path != "") && ::access(path, +mode) == 0;
}

bool File::exists(const String &path)
{
    return access(path, FileAccess::Exists);
}

void File::create(const String &path, FileMode mode)
{
    int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, +mode);
    if (fd == -1) CC_SYSTEM_RESOURCE_ERROR(errno, path);
    ::close(fd);
}

void File::establish(const String &path, FileMode fileMode, FileMode dirMode)
{
    if (path.find('/'))
        Dir::establish(path.cdUp(), dirMode);
    if (!File::exists(path))
        File::create(path, fileMode);
}

String File::createUnique(const String &path, FileMode mode, char placeHolder)
{
    assert(path.find(placeHolder));

    String candidate;

    for (Random random; true;) {
        candidate = path.copy();
        for (long i = 0, n = candidate.count(); i < n; ++i) {
            if (candidate[i] == placeHolder) {
                char r = static_cast<char>(random.get(0, 61));
                if ((0 <= r) && (r <= 9))
                    r += '0';
                else if ((10 <= r) && (r <= 35))
                    r += 'a' - 10;
                else if ((36 <= r) && (r <= 61))
                    r += 'A' - 36;
                candidate[i] = r;
            }
        }
        int fd = ::open(candidate, O_RDONLY|O_CREAT|O_EXCL, mode);
        if (fd == -1) {
            if (errno != EEXIST)
                CC_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            ::close(fd);
            break;
        }
    }

    return candidate;
}

String File::createTemp(FileMode mode)
{
    return createUnique("/tmp/" + str(::getpid()) + "_########", mode, '#');
}

void File::chown(const String &path, uid_t ownerId, gid_t groupId)
{
    if (::chown(path, ownerId, groupId) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::rename(const String &oldPath, const String &newPath)
{
    if (::rename(oldPath, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, oldPath);
}

void File::link(const String &oldPath, const String &newPath)
{
    if (::link(oldPath, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, newPath);
}

void File::unlink(const String &path)
{
    if (::unlink(path) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::symlink(const String &path, const String &newPath)
{
    if (::symlink(path, newPath) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

String File::readlink(const String &path)
{
    String buf = String::allocate(128);
    while (true) {
        long n = ::readlink(path, buf, buf.count());
        if (n == -1) return String{};
        if (n <= buf.count()) {
            buf.truncate(n);
            break;
        }
        buf = String::allocate(n);
    }
    return buf;
}

void File::clean(const String &path)
{
    FileStatus status{path};
    if (status.isValid()) {
        if (status.type() == FileType::Directory) {
            Dir::deplete(path);
            Dir::remove(path);
        }
        else
            File::unlink(path);
    }
}

String File::locate(const String &fileName, const List<String> &dirs, FileAccess accessFlags)
{
    String path;
    for (const String &dir: dirs) {
        String candidate = dir / fileName;
        if (File::access(candidate, accessFlags)) {
            path = candidate;
            break;
        }
    }
    return path;
}

String File::load(const String &path)
{
    File::establish(path);
    return File{path}.readAll();
}

void File::save(const String &path, const String &text)
{
    File::establish(path);
    File{path, FileOpen::WriteOnly|FileOpen::Truncate}.write(text);
}

File::File(const String &path, FileOpen flags, FileMode mode):
    File{new File::State{::open(path, +flags|O_CLOEXEC, +mode), path, flags}}
{
    if (fd() == -1) CC_SYSTEM_ERROR(errno, path);
}

void File::truncate(off_t length)
{
    if (::ftruncate(fd(), length) == -1)
        CC_SYSTEM_ERROR(errno, path());
}

off_t File::seek(off_t distance, Seek whence)
{
    off_t ret = ::lseek(fd(), distance, +whence);
    if (ret == -1) CC_SYSTEM_ERROR(errno, path());
    return ret;
}

bool File::isSeekable() const
{
    return ::lseek(fd(), 0, SEEK_CUR) != -1;
}

off_t File::currentOffset() const
{
    off_t ret = ::lseek(fd(), 0, SEEK_CUR);
    if (ret == -1) CC_SYSTEM_ERROR(errno, path());
    return ret;
}

void File::sync()
{
    if (::fsync(fd()) == -1)
        CC_SYSTEM_ERROR(errno, path());
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
    if (::fdatasync(fd()) == -1)
#else
    if (::fsync(fd()) == -1)
#endif
        CC_SYSTEM_ERROR(errno, path());
}

String File::readAll()
{
    off_t size = ::lseek(fd(), 0, SEEK_END);
    if (size == -1) return IoStream::readAll();

    if (::lseek(fd(), 0, SEEK_SET) == -1)
        CC_SYSTEM_ERROR(errno, path());

    String buffer = String::allocate(size);
    read(&buffer);
    return buffer;
}

String File::map()
{
    off_t len = ::lseek(fd(), 0, SEEK_END);
    if (len == -1)
        CC_SYSTEM_ERROR(errno, path());

    return map(0, len);
}

String File::map(off_t i0, off_t i1)
{
    assert(0 <= i0 && i0 <= i1);
    if (i0 == i1) return String{};

    off_t len = i1 - i0;
    bool writeable = +me().flags_ & (O_WRONLY|O_RDWR);

    void *p = ::mmap(
        nullptr,
        len,
        (writeable ? PROT_WRITE : 0) | PROT_READ,
        writeable ? MAP_SHARED : MAP_PRIVATE,
        fd(),
        i0
    );

    if (p == MAP_FAILED)
        CC_SYSTEM_DEBUG_ERROR(errno);

    return String{static_cast<char *>(p), static_cast<long>(len), 1};
}

void File::close()
{
    if (me().fd_ >= 0) {
        if (::close(me().fd_) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
        me().fd_ = -1;
    }
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

namespace cc {



} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Arguments>
#include <cc/Format>

namespace cc {

Arguments::Arguments(int argc, char *argv[]):
    me{argv[0]}
{
    for (int i = 1; i < argc; ++i)
    {
        String s = argv[i];
        bool isKeyValue = s.find('=');
        bool isFlag = s.startsWith('-') && s.count() >= 2 && (s.at(1) < '0' || '9' < s.at(1)) && s.at(1) != '.';
        if (!isFlag && !isKeyValue) {
            me().items.append(s);
            continue;
        }

        if (isFlag) s.trim("-");

        Variant value = true;
        if (isKeyValue) {
            List<String> parts = s.split('=');
            String name = parts.first();
            parts.popFront();
            String valueText{parts, '='};
            me().options.establish(name, Variant::read(valueText));
        }
        else {
            bool value = true;
            if (s.startsWith("no-")) {
                value = false;
                s = s.copy(3, s.count());
            }
            me().options.establish(s, value);
        }
    }

    if (me().options.find("h") || me().options.find("help") || me().options.find("?"))
        throw HelpRequest{};

    if (me().options.find("v") || me().options.find("version"))
        throw VersionRequest{};
}

void Arguments::validate(const Map<String, Variant> &prototype)
{
    for (Locator pos = me().options.head(); pos; ++pos)
    {
        String name = me().options.at(pos).key();
        Variant value = me().options.at(pos).value();

        Variant defaultValue;
        if (!prototype.lookup(name, &defaultValue))
            throw UsageError{Format{"No such option: \"%%\""} << name};

        if (defaultValue.is<void>()) continue;

        if (!value.sameTypeAs(defaultValue)) {
            if (value.is<long>() && defaultValue.is<bool>()) {
                long intValue = long(value);
                if (intValue == 0 || intValue == 1)
                    me().options.at(pos).value() = (intValue == 1);
            }
            else if (value.is<long>() && defaultValue.is<double>()) {
                me().options.at(pos).value() = double(long(value));
            }
            else if (value.is<long>() && defaultValue.is<String>()) {
                me().options.at(pos).value() = str(value);
            }
            else {
                throw UsageError{
                    Format{"Option \"%%\" expects type %% (got %%: %%)"}
                        << name
                        << defaultValue.typeName()
                        << value.typeName()
                        << value
                };
            }
        }
    }
}

void Arguments::override(InOut<Map<String, Variant>> config) const
{
    for (auto item: me().options)
        config().establish(item.key(), item.value());
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/StreamTap>

namespace cc {

struct StreamTap::State: public Stream::State
{
    State(const Stream &stream, const Stream &inputTap, const Stream &outputTap):
        stream_{stream},
        inputTap_{inputTap},
        outputTap_{outputTap}
    {}

    bool waitEstablished(int timeout) override
    {
        return stream_.waitEstablished(timeout);
    }

    bool wait(IoEvent event, int timeout) override
    {
        return stream_.wait(event, timeout);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        long n = stream_.read(buffer, maxFill);
        if (inputTap_) inputTap_.write(buffer, n);
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        stream_.write(buffer, fill);
        if (outputTap_) outputTap_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        stream_.write(buffers);
        if (outputTap_) outputTap_.write(buffers);
    }

    bool isDiscarding() const override
    {
        return stream_.isDiscarding();
    }

    Stream stream_;
    Stream inputTap_;
    Stream outputTap_;
};

StreamTap::StreamTap(const Stream &stream, const Stream &inputTap, const Stream &outputTap):
    Stream{new StreamTap::State{stream, inputTap, outputTap}}
{}

} // namespace cc
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourcePath>
#include <cc/ResourceManager>

namespace cc {

ResourcePath::ResourcePath(const String &path):
    path_{path}
{}

ResourcePath::operator String() const
{
    return ResourceManager{}.realPath(path_);
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TapBuffer>
#include <cc/Thread>
#include <cc/str>
#include <cc/DEBUG>

namespace cc {

struct TapBuffer::State: public LineBuffer::State
{
    State(const Stream &stream, const String &prefix):
        LineBuffer::State{stream, prefix}
    {}

    void write(const Bytes &buffer, long fill) override
    {
        List<Bytes> parts;
        long i = 0, i0 = 0;
        if (fill < 0) fill = buffer.count();
        for (long n = fill; i < n; ++i) {
            uint8_t ch = buffer.byteAt(i);
            if (ch <= 31 || 127 <= ch) {
                if (i0 < i) parts.append(buffer.copy(i0, i));
                i0 = i + 1;
                if (ch == 0x08) parts.append(String{"\\b", 2});
                else if (ch == 0x09) parts.append(String{"\\t", 2});
                else if (ch == 0x0A) parts.append(String{"\\n\n", 3});
                else if (ch == 0x0D) parts.append(String{"\\r", 2});
                else if (ch == 0x0C) parts.append(String{"\\f", 2});
                else {
                    String s { "\\xXX" };
                    const char *hex = "0123456789ABCDEF";
                    s.at(s.count() - 2) = hex[ch / 16];
                    s.at(s.count() - 1) = hex[ch % 16];
                    parts.append(s);
                    if (ch == 0) parts.append(String{"\n", 1});
                }
            }
        }
        if (parts.count() > 0) {
            if (i0 < i) parts.append(buffer.copy(i0, i));
            LineBuffer::State::write(parts.join());
        }
        else
            LineBuffer::State::write(buffer, fill);
    }

    String prefix() const override
    {
        return "(" + inum(Thread::id(), 62) + ") " + LineBuffer::State::prefix();
    }
};

TapBuffer::TapBuffer(const Stream &stream, const String &prefix):
    LineBuffer{new State{stream, prefix}}
{}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FileLock>
#include <cstring>

namespace cc {

FileLock::FileLock(const File &file, FileContention type, long long start, long long length, Seek whence):
    file_{file}
{
    std::memset(&lock_, 0, sizeof(lock_));
    lock_.l_type = +type;
    lock_.l_start = start;
    lock_.l_len = length;
    lock_.l_whence = +whence;
}

bool FileLock::tryAcquire()
{
    bool success = (::fcntl(file_.fd(), F_SETLK, &lock_) != -1);
    if (!success) {
        if ((errno != EACCES) && (errno != EAGAIN))
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return success;
}

void FileLock::acquire()
{
    if (::fcntl(file_.fd(), F_SETLKW, &lock_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void FileLock::release()
{
    lock_.l_type = F_UNLCK;
    if (::fcntl(file_.fd(), F_SETLK, &lock_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Dir>
#include <cc/DirWalker>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/Random>
#include <cc/str>
#include <cstring>
#include <dirent.h> // DIR, opendir, closedir, readdir

namespace cc {

bool Dir::access(const String &path, FileAccess mode)
{
    return ::access(path, +mode) == 0 && FileStatus{path}.type() == FileType::Directory;
}

bool Dir::exists(const String &path)
{
    return FileStatus{path}.type() == FileType::Directory;
}

void Dir::create(const String &path, FileMode mode)
{
    if (::mkdir(path, +mode) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void Dir::establish(const String &path, FileMode mode)
{
    List<String> missingDirs;
    for (
        String p = path;
        p.count() > 0 && p != "/";
        p = p.cdUp()
    ) {
        if (Dir::exists(p)) break;
        missingDirs.pushFront(p);
    }
    while (missingDirs.count() > 0) {
        const String &head = missingDirs.first();
        if (::mkdir(head, +mode) == -1) {
            if (!(errno == EEXIST && Dir::exists(head)))
                CC_SYSTEM_RESOURCE_ERROR(errno, head);
        }
        missingDirs.popFront();
    }
}

String Dir::createUnique(const String &path, FileMode mode, char placeHolder)
{
    assert(path.find(placeHolder));

    String candidate;

    for (Random random; true;) {
        candidate = path.copy();
        for (long i = 0, n = candidate.count(); i < n; ++i) {
            if (candidate[i] == placeHolder) {
                char r = static_cast<char>(random.get(0, 61));
                if ((0 <= r) && (r <= 9))
                    r += '0';
                else if ((10 <= r) && (r <= 35))
                    r += 'a' - 10;
                else if ((36 <= r) && (r <= 61))
                    r += 'A' - 36;
                candidate[i] = r;
            }
        }
        if (::mkdir(candidate, +mode) == -1) {
            if (!(errno == EEXIST && Dir::exists(candidate)))
                CC_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            break;
        }
    }

    return candidate;
}

String Dir::createTemp(FileMode mode)
{
    const char *tmpPrefix =
    #if defined __CYGWIN__ || defined __CYGWIN32__
        getenv("TMP");
    #else
         "/tmp/";
    #endif
    return createUnique(tmpPrefix + str(::getpid()) + "_########", mode, '#');
}

void Dir::remove(const String &path)
{
    if (::rmdir(path) == -1)
        CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

void Dir::deplete(const String &path)
{
    DirWalker walker{path};
    walker.setIgnoreHidden(false);
    walker.setFollowSymlink(false);
    walker.setDeleteOrder(true);

    String childPath;
    bool isDir = false;
    while (walker.read(&childPath, &isDir)) {
        if (isDir) Dir::remove(childPath);
        else File::unlink(childPath);
    }
}

struct Dir::State: public Object::State
{
    String path;
    DIR *dir { nullptr };

    ~State()
    {
        if (dir) ::closedir(dir);
    }
};

Dir::Dir(const String &path):
    Object{new State}
{
    me().path = path;
    me().dir = ::opendir(path);
    if (!me().dir) CC_SYSTEM_RESOURCE_ERROR(errno, path);
}

String Dir::path() const
{
    return me().path;
}

bool Dir::read(Out<String> name)
{
    struct dirent *entry = nullptr;

    while (true) {
        errno = 0;
        entry = ::readdir(me().dir);
        if (!entry && errno) CC_SYSTEM_DEBUG_ERROR(errno);
        if (entry) {
            if (std::strcmp(entry->d_name, ".") == 0) continue;
            if (std::strcmp(entry->d_name, "..") == 0) continue;
            name = entry->d_name;
        }
        break;
    }

    return entry;
}

Stream Dir::openFile(const String &path, FileOpen flags)
{
    String targetPath = path.isRelativePath() ? (me().path / path) : path;
    return File{targetPath, flags};
}

Dir::State &Dir::me()
{
    return Object::me.as<State>();
}

const Dir::State &Dir::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/GroupInfo>
#include <cc/UserInfo>
#include <sys/types.h>
#include <unistd.h> // sysconf
#include <grp.h>

namespace cc {

GroupInfo::State::State(const struct group &info):
    id{static_cast<int>(info.gr_gid)},
    name{info.gr_name}
{
    for (char **pcs = info.gr_mem; *pcs; ++pcs)
        members << String{*pcs};
}

GroupInfo::GroupInfo(gid_t id)
{
    int bufSize = ::sysconf(_SC_GETGR_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct group space;
    struct group *info = nullptr;
    int ret = ::getgrgid_r(id, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    if (info) Object::me = new State{*info};
}

GroupInfo::GroupInfo(const String &name)
{
    int bufSize = ::sysconf(_SC_GETGR_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct group space;
    struct group *info = nullptr;
    int ret = ::getgrnam_r(name, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_RESOURCE_ERROR(ret, name);
    if (info) Object::me = new State{*info};
}

bool GroupInfo::checkMembership(const UserInfo &user) const
{
    return user.groupId() == me().id || me().members.find(user.loginName());
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/WaitCondition>
#include <cassert>
#include <cmath>

namespace cc {

WaitCondition::WaitCondition()
{
    int ret = pthread_cond_init(&cond_, nullptr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

WaitCondition::~WaitCondition()
{
    #ifndef NDEBUG
    int ret =
    #endif
    pthread_cond_destroy(&cond_);
    #ifndef NDEBUG
    assert(ret == 0);
    #endif
}

void WaitCondition::wait(Mutex &mutex)
{
    int ret = -1;
    do {
        ret = pthread_cond_wait(&cond_, &mutex.mutex_);
    } while (ret == EINTR);

    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

bool WaitCondition::waitUntil(double time, Mutex &mutex)
{
    bool success = true;
    struct timespec ts;
    double sec = 0;
    ts.tv_nsec = std::modf(time, &sec) * 1e9;
    ts.tv_sec = sec;
    int ret = -1;
    do {
        ret = pthread_cond_timedwait(&cond_, &mutex.mutex_, &ts);
    } while (ret == EINTR);

    if (ret != 0) {
        if (ret == ETIMEDOUT)
            success = false;
        else
            CC_SYSTEM_DEBUG_ERROR(ret);
    }
    return success;
}

void WaitCondition::signal()
{
    int ret = pthread_cond_signal(&cond_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void WaitCondition::broadcast()
{
    int ret = pthread_cond_broadcast(&cond_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IoStream>
#include <sys/types.h>
#include <sys/uio.h> // readv, writev
#include <sys/ioctl.h> // ioctl
#include <sys/socket.h> // socketpair, shutdown, SHUT_*
#include <termios.h> // struct termios, ECHO
#include <unistd.h> // read, write, select, sysconf, isatty
#include <poll.h> // poll, POLLIN, POLLOUT

namespace cc {

IoStream::State::~State()
{
    if (fd_ >= 3) ::close(fd_);
}

bool IoStream::State::wait(IoEvent event, int timeout)
{
    struct pollfd fds;
    fds.fd = fd_;
    fds.events = 0;
    if (event & IoEvent::ReadyRead) fds.events |= POLLIN;
    if (event & IoEvent::ReadyWrite) fds.events |= POLLOUT;

    if (timeout < 0) timeout = -1;
    int ret = -1;
    do ret = ::poll(&fds, 1, timeout);
    while (ret == -1 && errno == EINTR);
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    assert(ret == 0 || ret == 1);
    assert(timeout != -1 || ret == 1);

    return ret == 1;
}

long IoStream::State::read(Out<Bytes> buffer, long maxFill)
{
    long n = (maxFill < 0 || buffer().count() < maxFill) ? buffer().count() : maxFill;
    long m = -1;
    do m = ::read(fd_, buffer(), n);
    while (m == -1 && errno == EINTR);
    if (m == -1) {
        if (errno == EWOULDBLOCK) throw Timeout{};
        if (errno == ECONNRESET || errno == EPIPE) throw InputExhaustion{};
        #if defined __CYGWIN__ || defined __CYGWIN32__
        if (errno == ECONNABORTED) return 0;
        #endif
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return m;
}

void IoStream::State::write(const Bytes &buffer, long fill)
{
    const uint8_t *p = buffer.bytes();
    long n = (0 < fill && fill < buffer.count()) ? fill : buffer.count();

    while (n > 0) {
        long m = -1;
        do m = ::write(fd_, p, n);
        while (m == -1 && errno == EINTR);
        if (m == -1) {
            if (errno == EWOULDBLOCK) throw Timeout{};
            if (errno == ECONNRESET || errno == EPIPE) throw OutputExhaustion{};
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
        p += m;
        n -= m;
    }
}

void IoStream::State::write(const List<Bytes> &buffers)
{
    const int n = buffers.count();

    if (n <= 0) return;

    if (scatterLimit_ <= 0) {
        write(Bytes{buffers});
        return;
    }

    {
        long total = 0;
        for (long i = 0; i < n; ++i)
            total += buffers.at(i).count();

        if (total <= scatterLimit_) {
            write(Bytes{buffers});
            return;
        }
    }

    using IoItem = struct iovec;

    struct IoVector {
        IoVector(long n): items{new IoItem[n]} {}
        ~IoVector() { delete[] items; }
        IoItem *items;
    };

    IoVector vector{n};
    IoItem *iov = vector.items;

    for(auto pos = buffers.head(); pos; ++pos) {
        const Bytes &buffer = buffers.at(pos);
        iov[+pos].iov_base = const_cast<uint8_t *>(buffer.items());
        iov[+pos].iov_len = buffer.count();
    }

    const int iovMax = ::sysconf(_SC_IOV_MAX);

    for (int i = 0; i < n;) {
        int m = n - i;
        if (m > iovMax) m = iovMax;
        ssize_t ret = -1;
        do ret = ::writev(fd_, iov + i, m);
        while (ret == -1 && errno == EINTR);
        if (ret == -1) {
            if (errno == EWOULDBLOCK) throw Timeout{};
            if (errno == ECONNRESET || errno == EPIPE) throw OutputExhaustion{};
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
        i += m;
    }
}

IoStream &IoStream::input()
{
    static thread_local IoStream stream{0};
    return stream;
}

IoStream &IoStream::output()
{
    static thread_local IoStream stream{1};
    return stream;
}

IoStream &IoStream::error()
{
    static thread_local IoStream stream{2};
    return stream;
}

void IoStream::pair(Out<IoStream> a, Out<IoStream> b)
{
    int fd[2] = { 0, 0 };

    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    a = IoStream{fd[0]};
    b = IoStream{fd[1]};
}

IoStream::IoStream(int fd):
    Stream{new IoStream::State{fd}}
{}

void IoStream::shutdown(IoShutdown mode)
{
    if (fd() >= 0) {
        int how = 0;
        if (mode == IoShutdown::Read) how |= SHUT_RD;
        if (mode == IoShutdown::Write) how |= SHUT_WR;
        if (mode == IoShutdown::Full) how |= SHUT_RDWR;
        if (::shutdown(fd(), how) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void IoStream::duplicateTo(IoStream &other)
{
    if (::dup2(fd(), other.me().fd_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

IoStream IoStream::duplicate() const
{
    int fd2 = ::dup(fd());
    if (fd2 == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return IoStream{fd2};
}

bool IoStream::isInteractive() const
{
    return ::isatty(fd());
}

void IoStream::echo(bool on)
{
    struct termios settings;

    if (::tcgetattr(fd(), &settings) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (((settings.c_lflag & ECHO) != 0) == on) return;

    if (on)
        settings.c_lflag |= ECHO;
    else
        settings.c_lflag &= ~ECHO;

    if (::tcsetattr(fd(), TCSAFLUSH, &settings) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int IoStream::ioctl(int request, void *arg)
{
    int value = ::ioctl(fd(), request, arg);

    if (value == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    return value;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PluginDir>
#include <cc/Dir>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/stdio>

namespace cc {

PluginDir::PluginDir(const String &path):
    Object{new State{path}}
{
    Dir dir{me().path_};
    for (const String &name: dir) {
        String path = me().path_ / name;
        try {
            FileStatus status{path, false};
            if (status.type() == FileType::Symlink) {
                path = File::readlink(path);
                if (path.isRelativePath())
                    path = me().path_.cd(path);
            }
            else if (status.type() == FileType::Regular)
                ;
            else
                continue;

            Plugin plugin{path};
            me().loadedPlugins_.insert(plugin.name(), plugin);
            onLoaded(plugin);
        }
        catch (Exception &ex) {
            onError(path, ex.message());
        }
    }

    getPluginDirs() << *this;
}

void PluginDir::onError(const String &pluginPath, const String &errorMessage)
{
    ferr() << "Failed to load plugin " << pluginPath << ": " << errorMessage << nl;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/exceptions>
#include <cc/str>

namespace cc {

String OutputExhaustion::message() const
{
    return "Output exhaustion";
}

String InputExhaustion::message() const
{
    return "Input exhaustion";
}

String Timeout::message() const
{
    return "Operation timeout";
}

String HelpRequest::message() const
{
    return "No help, yet ...";
}

String VersionRequest::message() const
{
    return "Application is not versioned, yet ...";
}

String PropertyBindingLoop::message() const
{
    return "Property binding loop detected";
}

String CommandNotFound::message() const
{
    return List<String>{} << "Command not found: '" << command_ << "'";
}

String DebugError::message() const
{
    return List<String>{} << source_ << ":" << str(line_) << ": " << reason_;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf16Source>

namespace cc {

bool Utf16Source::read(Out<char32_t> ch)
{
    bool hasMore = byteSource_.prefetch();

    if (hasMore) {
        uint16_t w = byteSource_.readUInt16();
        if (w == 0xFFFE && firstTime_) {
            firstTime_ = false;
            byteSource_.setEndian((byteSource_.endian() == ByteOrder::BigEndian) ? ByteOrder::LittleEndian : ByteOrder::BigEndian);
            w = byteSource_.readUInt16();
        }
        char32_t u = w;
        if ((0xD800 <= u) && (u < 0xDC00)) {
            uint16_t w = byteSource_.readUInt16();
            if (!((0xDC00 <= w) && (w < 0xE000))) throw DecodingError{};
            u = 0x10000 + (((u - 0xD800) << 10) | (w - 0xDC00));
        }
        else if (0x10FFFF < u) throw DecodingError{};

        ch << u;
    }

    return hasMore;
}

String Utf16Source::DecodingError::message() const
{
    return "UTF-16 error: failed to decode input bytes";
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf8Sink>

namespace cc {

void Utf8Sink::write(char32_t ch)
{
    if (ch < 0x80) { // ASCII range: 0xxxxxxx
        byteSink_.writeUInt8(ch);
    }
    else if (ch < 0x800) { // two-byte codes: 110yyyxx | 10xxxxx
        byteSink_.writeUInt8((ch >> 6) | 0xC0);   // 0xC = (1100)2, code prefix: (110)2
        byteSink_.writeUInt8((ch & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
    }
    else if (ch < 0x10000) { // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
        if ((0xD800 <= ch) && (ch <= 0xDFFF)) throw SurrogatePairEncodingError{};
        else if ((0xFDD0 <= ch) && (ch <= 0xFDEF)) throw NonCharacterEncodingError{};
        byteSink_.writeUInt8((ch >> 12) | 0xE0);         // 0xE = (1110)2, code prefix: (1110)2
        byteSink_.writeUInt8(((ch >> 6) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
        byteSink_.writeUInt8((ch & 0x3F) | 0x80);        // 0x8 = (1000)2, code prefix: (10)2
    }
    else if (ch < 0x110000) { // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
        if ((ch & 0xFFFE) == 0xFFFE) throw NonCharacterEncodingError{};
        byteSink_.writeUInt8((ch >> 18) | 0xF0);          // 0xF = (1111)2, code prefix: (11110)2
        byteSink_.writeUInt8(((ch >> 12) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
        byteSink_.writeUInt8(((ch >> 6) & 0x3F) | 0x80);  // 0x8 = (1000)2, code prefix: (10)2
        byteSink_.writeUInt8((ch & 0x3F) | 0x80);         // 0x8 = (1000)2, code prefix: (10)2
    }
    else {
        throw LargeCodePointEncodingError{};
    }
}

String Utf8Sink::SurrogatePairEncodingError::message() const
{
    return "UTF-8 error: encoding of surrogate pairs is not allowed (0xD800..0xDFFF)";
}

String Utf8Sink::NonCharacterEncodingError::message() const
{
    return "UTF-8 error: encoding of non-characters is not allowed (0xFDD0..0xFFFE, 0x*FFF(E|F))";
}

String Utf8Sink::LargeCodePointEncodingError::message() const
{
    return "UTF-8 error: code point to large (>= 0x110000)";
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/KernelInfo>
#include <sys/utsname.h>

namespace cc {

KernelInfo::KernelInfo():
    Singleton{instance<State>()}
{}

KernelInfo::State::State()
{
    struct utsname data;
    if (::uname(&data) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    name_ = data.sysname;
    release_ = data.release;
    version_ = data.version;
    machine_ = data.machine;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FileStatus>
#include <cstring>
#include <cmath>

namespace cc {

FileStatus::FileStatus(const String &path, bool followSymlink)
{
    me().path = path;
    me().followSymlink = followSymlink;
    update();
}

void FileStatus::update()
{
    StructStat &buf = static_cast<StructStat &>(me());
    std::memset(&buf, 0, sizeof(StructStat));
    if (me().path == "") {
        me().isValid = false;
    }
    else {
        int ret = me().followSymlink ? ::stat(me().path, &buf) : ::lstat(me().path, &buf);
        if (ret == -1) {
            if ((errno == ENOENT) || (errno == ENOTDIR))
                me().isValid = false;
            else
                CC_SYSTEM_ERROR(errno, me().path);
        }
        else
            me().isValid = true;
    }
}

void FileStatus::setTimes(double lastAccess, double lastModified)
{
    struct timespec tv[2];
    double sec;
    tv[0].tv_nsec = std::modf(lastAccess, &sec) * 1e9;
    tv[0].tv_sec = sec;
    tv[1].tv_nsec = std::modf(lastModified, &sec) * 1e9;
    tv[1].tv_sec = sec;
    if (::utimensat(AT_FDCWD, me().path, tv, AT_SYMLINK_NOFOLLOW * (!me().followSymlink)) == -1)
        CC_SYSTEM_ERROR(errno, me().path);

    me().st_atim = tv[0];
    me().st_mtim = tv[1];
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ByteSink>
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf16Sink>

namespace cc {

String Utf16Sink::SurrogatePairEncodingError::message() const
{
    return "UTF-16 error: encoding surrogate pairs is not allowed (0xD800..0xDFFF)";
}

String Utf16Sink::ByteOrderMarkEncodingError::message() const
{
    return "UTF-16 error: encoding the BOM is not allowed (0xFEFF, 0xFFFE)";
}

String Utf16Sink::LargeCodePointEncodingError::message() const
{
    return "UTF-16 error: code point to large (>= 0x110000)";
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourceGuard>
#include <cc/ResourceContext>

namespace cc {

ResourceGuard::ResourceGuard(const String &resource)
{
    ResourceContext{}.push(resource);
}

ResourceGuard::~ResourceGuard()
{
    ResourceContext{}.pop();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MountTable>
#include <cc/File>
#include <cc/DEBUG>

namespace cc {

MountPoint::MountPoint(const String &line):
    parts_{line.split(' ')}
{
    while (parts_.count() < 4) parts_.append(String{});
}

MountTable::MountTable(const String &path)
{
    readLines(loadText(path));
}

MountTable MountTable::read(const String &text)
{
    MountTable table;
    table.readLines(text);
    return table;
}

String MountTable::loadText(const String &path)
{
    String text;
    {
        File file;
        if (path == "") {
            #ifdef __linux
            try { file = File{"/proc/self/mounts"}; }
            catch (...)
            #endif
            {
                file = File{"/etc/mtab"};
            }
        }
        else
            file = File{path};

        text = file.readAll();
    }
    return text;
}

void MountTable::readLines(const String &text)
{
    for (String line: text.split('\n')) {
        line.simplify();
        if (line.count() == 0) continue;
        if (line.at(0) == '#') continue;
        (*this) << MountPoint{line};
    }
}

String MountTable::toString() const
{
    List<String> lines;
    for (const auto &entry: *this)
        lines << entry.toString();
    return lines;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Command>
#include <cc/LocalChannel>
#include <cc/Process>
#include <cc/Thread>
#include <cc/File>
#include <cstring>
#include <spawn.h>

namespace cc {

#define CC_SPAWN_CALL(call) \
{\
    int ret = call; \
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret); \
}

struct Command::State: public Object::State
{
    State(const String &command = String{}):
        command_{command}
    {
        CC_SPAWN_CALL(posix_spawnattr_init(&spawnAttributes_));
        CC_SPAWN_CALL(posix_spawn_file_actions_init(&fileActions_));
    }

    ~State()
    {
        posix_spawn_file_actions_destroy(&fileActions_);
        posix_spawnattr_destroy(&spawnAttributes_);
    }

    void enableSpawnFlag(short flag)
    {
        short flags = 0;
        CC_SPAWN_CALL(posix_spawnattr_getflags(&spawnAttributes_, &flags));
        flags |= flag;
        CC_SPAWN_CALL(posix_spawnattr_setflags(&spawnAttributes_, flags));
    }

    String command_;
    String execPath_;
    List<String> args_;
    Map<String> envMap_;
    bool inheritEnvMap_ { true };

    int spawnFlags_ { 0 };
    posix_spawnattr_t spawnAttributes_;
    posix_spawn_file_actions_t fileActions_;
    bool groupLead_ { false };
    List<IoChannel> ioChannels_;
    List<IoStream> ioStreams_;
    IoStream standardStreams_[3];

    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    #else
    String cwd_;
    #endif
    #else
    String cwd_;
    #endif
};

Command::Command():
    Object{new State}
{}

Command::Command(const String &command):
    Object{new State{command}}
{}

Command &Command::args(const List<String> &args)
{
    me().args_ = args;
    return *this;
}

Command &Command::env(const Map<String> &map)
{
    me().envMap_ = map;
    me().inheritEnvMap_ = false;
    return *this;
}

Command &Command::group(int groupId)
{
    CC_SPAWN_CALL(posix_spawnattr_setpgroup(&me().spawnAttributes_, groupId));
    me().enableSpawnFlag(POSIX_SPAWN_SETPGROUP);
    me().groupLead_ = (groupId == 0);
    return *this;
}

Command &Command::signalMask(const SignalSet &mask)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigmask(&me().spawnAttributes_, mask));
    me().enableSpawnFlag(POSIX_SPAWN_SETSIGMASK);
    return *this;
}

Command &Command::signalDefault(const SignalSet &set)
{
    CC_SPAWN_CALL(posix_spawnattr_setsigdefault(&me().spawnAttributes_, set));
    me().enableSpawnFlag(POSIX_SPAWN_SETSIGDEF);
    return *this;
}

Command &Command::workDir(const String &path)
{
    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    CC_SPAWN_CALL(posix_spawn_file_actions_addchdir_np(&me().fileActions_, path));
    #else
    me().cwd_ = path;
    #endif
    #else
    me().cwd_ = path;
    #endif
    return *this;
}

Command &Command::io(int fd, const IoChannel &channel)
{
    me().ioChannels_.append(channel);
    if (0 <= fd && fd <= 2) me().standardStreams_[fd] = channel;
    CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&me().fileActions_, channel.slaveFd(), fd));
    return *this;
}

Command &Command::io(int fd, const IoStream &stream)
{
    if (0 <= fd && fd <= 2) me().standardStreams_[fd] = stream;
    else me().ioStreams_.append(stream);
    if (stream.fd() != fd)
        CC_SPAWN_CALL(posix_spawn_file_actions_adddup2(&me().fileActions_, stream.fd(), fd));
    return *this;
}

void Command::start(Process &process)
{
    /// automatically setup I/O redirection if not requested otherwise

    if (
        !me().standardStreams_[0] ||
        !me().standardStreams_[1] ||
        !me().standardStreams_[2]
    ) {
        LocalChannel channel;
        if (!me().standardStreams_[0]) io(0, channel);
        if (!me().standardStreams_[1]) io(1, channel);
        if (!me().standardStreams_[2]) io(2, channel);
    }

    /// locate executable and prepare argument list

    String execPath;
    List<String> args = me().args_;

    if (me().command_ != "") {
        String cmd = me().command_.copy();
        if (cmd.startsWith(' ') || cmd.endsWith(' '))
            cmd.trim();
        if (!cmd.find(' ')) execPath = cmd;
        if (args.count() == 0) {
            cmd.simplify();
            args = cmd.split(' ');
        }
    }

    if (execPath == "" && args.count() > 0) execPath = args.at(0);

    if (execPath != "") {
        if (execPath.find('/')) {
            if (!File::access(execPath, FileAccess::Execute)) throw CommandNotFound{execPath};
        }
        else {
            String path = File::locate(execPath, Process::env("PATH").split(':'), FileAccess::Execute);
            if (path == "") throw CommandNotFound{execPath};
            execPath = path;
        }
    }

    /// prepare argument list and environment map

    const int argc = (args.count() > 0) ? args.count() : 1;
    char **argv = new char*[argc + 1];
    {
        if (args.count() > 0) {
            for (int i = 0; i < args.count(); ++i)
                argv[i] = String{args.at(i)};
        }
        else {
            argv[0] = execPath;
        }
        argv[argc] = 0;
    }

    char **envp = nullptr;
    if (!me().inheritEnvMap_) {
        const Map<String> &envMap = me().envMap_;
        const int n = envMap.count();
        envp = new char*[n + 1];
        int i = 0;
        for (auto &pair: envMap) {
            int n = pair.key().count() + pair.value().count() + 2;
            char *s = static_cast<char *>(::malloc(n));
            std::memcpy(s, pair.key().chars(), pair.key().count());
            s[pair.key().count()] = '=';
            std::memcpy(s + pair.key().count() + 1, pair.value().chars(), pair.value().count());
            s[n - 1] = '\0';
            envp[i] = s;
            ++i;
        }
        envp[n] = nullptr;
    }

    /// spawn new child process

    #ifdef __GLIBC__
    #if __GLIBC_PREREQ(2, 29)
    #else
    class CwdGuard {
    public:
        CwdGuard(const String &cwd):
            cwdSaved_{cwd != "" ? Process::cwd() : String{}}
        {
            if (cwd != "") Process::setCwd(cwd);
        }
        ~CwdGuard()
        {
            if (cwdSaved_ != "") Process::setCwd(cwdSaved_);
        }
    private:
        String cwdSaved_;
    };
    CwdGuard guard{me().cwd_};
    #endif
    #endif

    process.me().pid_ = -1;
    do {
        int ret = ::posix_spawn(&process.me().pid_, execPath, &me().fileActions_, &me().spawnAttributes_, argv, envp ? envp : ::environ);
        if (ret != 0) {
            if (ret == EAGAIN) {
                Thread::sleep(1);
                continue;
            }
            CC_SYSTEM_DEBUG_ERROR(ret);
        }
    }
    while (false);

    delete[] argv;
    if (envp) {
        for (int i = 0; envp[i]; ++i)
            ::free(envp[i]);
        delete[] envp;
    }

    process.me().groupLead_ = me().groupLead_;

    for (int i = 0; i <= 2; ++i)
        process.me().standardStreams_[i] = me().standardStreams_[i];

    for (IoChannel &channel: me().ioChannels_)
        channel.me().onStart();
}

Command::State &Command::me()
{
    return Object::me.as<State>();
}

const Command::State &Command::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HexDump>
#include <cc/Format>
#include <cc/IoStream>
#include <cc/CaptureSink>

namespace cc {

struct HexDump::State final: public Stream::State
{
    State(const Stream &sink):
        sink_{sink},
        buffer_{Bytes::allocate(16)}
    {
        if (!sink_) sink_ = IoStream::output();
    }

    ~State()
    {
        flush();
    }

    void write(const Bytes &data, long fill = -1) override
    {
        if (fill < 0) fill = data.count();
        for (long i = 0; i < fill; ++i) {
            writeByte(data.at(i));
        }
    }

    void writeByte(uint8_t ch)
    {
        if (i_ == buffer_.count()) flush();
        buffer_.at(i_) = ch;
        ++i_;
    }

    void flush()
    {
        Format f{sink_};
        f << hex(offset_, 8) << "  ";
        for (long j = 0; j < buffer_.count(); ++j) {
            if (j < i_) {
                uint8_t &ch = buffer_.at(j);
                f << hex(ch, 2);
                if (ch < 0x20 || 0x7F <= ch) ch = '.';
            }
            else f << "  ";
            f << (((j + 1) % 8 == 0) ? "  " : " ");
        }
        f << "|" << buffer_.select(0, i_) << "|" << nl;
        offset_ += i_;
        i_ = 0;
    }

    Stream sink_;
    Bytes buffer_;
    long i_ { 0 };
    long long offset_ { 0 };
};

HexDump::HexDump(const Stream &sink):
    Stream{new State{sink}}
{}

String hexdump(const Bytes &data)
{
    CaptureSink capture;
    HexDump{capture}.write(data);
    return capture.collect();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IoMonitor>
#include <cc/Map>
#include <poll.h>

namespace cc {

struct IoMonitor::State final: public Object::State
{
    Map<int, IoActivity> subjects;
    Array<struct pollfd> fds;
    bool dirty { true };
};

IoMonitor::IoMonitor():
    Object{new State}
{}

void IoMonitor::watch(const IoStream &target, IoEvent mask)
{
    me().dirty = true;
    me().subjects.establish(target.fd(), IoActivity{target, mask});
}

void IoMonitor::unwatch(const IoStream &target)
{
    me().dirty = true;
    me().subjects.remove(target.fd());
}

bool IoMonitor::wait(const Fun<void(const IoActivity &)> &onReady, int timeout)
{
    assert(me().subjects.count() > 0);

    if (me().dirty) {
        me().dirty = false;
        me().fds = Array<struct pollfd>::allocate(me().subjects.count());
        long i = 0;
        for (const auto &pair: me().subjects) {
            struct pollfd &p = me().fds[i++];
            p.fd = pair.value().target().fd();
            p.events = 0;
            if (pair.value().mask() & IoEvent::ReadyRead) p.events |= POLLIN;
            if (pair.value().mask() & IoEvent::ReadyWrite) p.events |= POLLOUT;
            if (pair.value().mask() & IoEvent::Established) p.events = POLLIN|POLLOUT;
        }
    }

    int n = -1;
    do n = ::poll(me().fds.items(), me().fds.count(), (timeout < 0) ? -1 : timeout);
    while (n == -1 && errno == EINTR);
    if (n < 0) CC_SYSTEM_DEBUG_ERROR(errno);

    bool ready = (n > 0);

    if (ready) {
        for (int i = 0; i < me().fds.count(); ++i) {
            const struct pollfd &p = me().fds[i];
            if (p.revents != 0) {
                IoActivity &activity = me().subjects(p.fd);
                IoEvent &event = activity.me().event_;
                event = IoEvent::None;
                if (p.revents & POLLIN) event |= IoEvent::ReadyRead;
                if (p.revents & POLLOUT) event |= IoEvent::ReadyWrite;
                onReady(activity);
                if (--n == 0) break;
            }
        }
    }

    return ready;
}

IoMonitor::State &IoMonitor::me()
{
    return Object::me.as<State>();
}

const IoMonitor::State &IoMonitor::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Thread>
#include <cc/System>
#include <cmath> // std::modf(3)
#include <utility>
#include <cassert>
#include <limits.h> // PTHREAD_STACK_*
#include <time.h> // nanosleep(2)
#include <pthread.h>

namespace cc {

struct Thread::State: public Object::State
{
    static void *bootstrap(void *self)
    {
        static_cast<State *>(self)->f_();
        return self;
    }

    State(Fun<void()> &&f, long stackSize):
        f_{f}
    {
        {
            int ret = pthread_attr_init(&attr_);
            if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
        }

        if (stackSize > 0) {
            if (stackSize < PTHREAD_STACK_MIN) stackSize = PTHREAD_STACK_MIN;
            int ret = pthread_attr_setstacksize(&attr_, stackSize);
            if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
        }
    }

    inline void start()
    {
        int ret = pthread_create(&tid_, &attr_, &bootstrap, static_cast<void *>(this));
        if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    }

    pthread_attr_t attr_;
    pthread_t tid_;
    Fun<void()> f_;
};

Thread::Thread(Fun<void()> &&f, long stackSize):
    Object{new State{move(f), stackSize}}
{}

void Thread::start()
{
    assert(!isNull());
    me().start();
}

void Thread::wait()
{
    int ret = pthread_join(me().tid_, nullptr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::kill(Signal signal)
{
    int ret = pthread_kill(me().tid_, +signal);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

bool Thread::isRunning() const
{
    int ret = pthread_kill(me().tid_, 0);
    if (ret != 0 && ret != ESRCH) CC_SYSTEM_DEBUG_ERROR(ret);
    return (ret == 0);
}

uint64_t Thread::id()
{
    return static_cast<uint64_t>(pthread_self());
}

void Thread::sleep(double duration)
{
    struct timespec rqtp;
    double sec = 0;
    rqtp.tv_nsec = std::modf(duration, &sec) * 1e9;
    rqtp.tv_sec = sec;
    while (::nanosleep(&rqtp, &rqtp) == -1) {
        if (errno != EINTR)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void Thread::sleepUntil(double time)
{
    double now = System::now();
    if (time <= now) return;
    sleep(now - time);
}

void Thread::blockSignals(const SignalSet &set)
{
    int ret = pthread_sigmask(SIG_BLOCK, set, nullptr/*oldset*/);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Thread::unblockSignals(const SignalSet &set)
{
    int ret = pthread_sigmask(SIG_UNBLOCK, set, nullptr/*oldset*/);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

SignalSet Thread::signalMask()
{
    SignalSet set;
    pthread_sigmask(/*how*/0, /*new_set*/nullptr, /*old_set*/set);
    return set;
}

int Thread::getMinPriority(SchedulingPolicy policy)
{
    int ret = sched_get_priority_min(static_cast<int>(policy));
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return ret;
}

int Thread::getMaxPriority(SchedulingPolicy policy)
{
    int ret = sched_get_priority_max(static_cast<int>(policy));
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    return ret;
}

void Thread::setSchedulingPolicy(SchedulingPolicy policy, int priority)
{
    struct sched_param param;
    std::memset(&param, 0, sizeof(param));
    param.sched_priority = priority;
    int ret = sched_setscheduler(0, +policy, &param);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

Thread::State &Thread::me()
{
    return Object::me.as<State>();
}

const Thread::State &Thread::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Random>
#include <time.h> // clock(2)

namespace cc {

Random::Random(int seed):
    x_{static_cast<uint32_t>(seed)}
{
    if (seed < 0) {
        while (x_ <= 1) {
            x_ = static_cast<uint32_t>(::clock());
        }
    }
    else {
        x_ = seed + 2;
        if (x_ <= 1) x_ += 2; // overflow check
    }
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Mutex>
#include <cassert>

namespace cc {

Mutex::Mutex()
{
    pthread_mutexattr_t *pattr = nullptr;
    int ret;
    #ifndef NDEBUG
    pthread_mutexattr_t attr;
    pattr = &attr;
    ret = pthread_mutexattr_init(&attr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #endif
    ret = pthread_mutex_init(&mutex_, pattr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #ifndef NDEBUG
    ret = pthread_mutexattr_destroy(&attr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #endif
}

Mutex::~Mutex()
{
    #ifndef NDEBUG
    int ret =
    #endif
    pthread_mutex_destroy(&mutex_);
    #ifndef NDEBUG
    assert(ret == 0);
    #endif
}

bool Mutex::tryAcquire()
{
    int ret = -1;
    while (true) {
        ret = pthread_mutex_trylock(&mutex_);
        if (ret != EINTR) break;
    }
    if ((ret != 0) && (ret != EBUSY))
        CC_SYSTEM_DEBUG_ERROR(ret);
    return ret != EBUSY;
}

void Mutex::acquire()
{
    int ret = -1;
    while (true) {
        ret = pthread_mutex_lock(&mutex_);
        if (ret != EINTR) break;
    }
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Mutex::release()
{
    int ret = pthread_mutex_unlock(&mutex_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

} // namespace cc
/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourceManager>
#include <cc/Map>
#include <cc/Prefix>

namespace cc {

struct ResourceManager::State final: public Singleton::State
{
    void addDirectory(const String &realPrefix, const String &resourcePrefix)
    {
        pathMap_.establish(resourcePrefix != "" ? resourcePrefix : realPrefix.fileName(), realPrefix);
    }

    bool lookup(const String &resourcePath, Out<String> realPath) const
    {
        if (!resourcePath.startsWith(":/")) return resourcePath;

        Locator target;
        bool found = pathMap_.find(Prefix{resourcePath, 2}, &target);

        if (found) {
            String resourcePrefix = pathMap_.at(target).key();
            String realPrefix = pathMap_.at(target).value();
            String realSuffix = resourcePath.copy(2 + resourcePrefix.count(), resourcePath.count());
            realPath = realPrefix + realSuffix;
        }

        return found;
    }

    Map<String, String> pathMap_;
};

ResourceManager::ResourceManager():
    Singleton{instance<State>()}
{}

ResourceManager &ResourceManager::addDirectory(const String &realPrefix, const String &resourcePrefix)
{
    me().addDirectory(realPrefix, resourcePrefix);
    return *this;
}

bool ResourceManager::lookup(const String &resourcePath, Out<String> realPath) const
{
    return me().lookup(resourcePath, &realPath);
}

String ResourceManager::realPath(const String &resourcePath) const
{
    String realPath;
    lookup(resourcePath, &realPath);
    return realPath;
}

ResourceManager::State &ResourceManager::me()
{
    return Object::me.as<State>();
}

const ResourceManager::State &ResourceManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LocalChannel>
#include <sys/socket.h> // socketpair(2)
#include <unistd.h> // close(2)
#include <errno.h>

namespace cc {

LocalChannel::State::State()
{
    int fd[2] = { 0, 0 };
    CC_SYSCALL(::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd));

    fd_ = fd[0];
    slaveFd_ = fd[1];
}

LocalChannel::State::~State()
{
    if (slaveFd_ != -1) ::close(slaveFd_);
}

void LocalChannel::State::onStart()
{
    if (slaveFd_ != -1) {
        ::close(slaveFd_);
        slaveFd_ = -1;
    }
}

LocalChannel::LocalChannel():
    IoChannel{new LocalChannel::State}
{}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/str>
#include <cc/Queue>
#include <cc/Utf8Sink>
#include <cc/binary>
#include <cc/math>

namespace cc {

String hex(const Bytes &data)
{
    long n = data.count();
    if (n == 0) return String{};

    String s2 = String::allocate(2 * n);
    long j = 0;
    for (long i = 0; i < n; ++i) {
        unsigned char ch = data.item<unsigned char>(i);
        int d0 = (ch >> 4) & 0xf;
        int d1 = ch & 0xf;
        if ((0 <= d0) && (d0 < 10)) s2[j++] = d0 + '0';
        else s2[j++] = (d0 - 10) + 'a';
        if ((0 <= d1) && (d1 < 10)) s2[j++] = d1 + '0';
        else s2[j++] = (d1 - 10) + 'a';
    }

    return s2;
}

uint8_t readHexDigit(char ch)
{
    uint8_t digit = 0;
    if ('0' <= ch && ch <= '9') digit = ch - '0';
    else if ('A' <= ch && ch <= 'F') digit = ch - 'A' + 10;
    else if ('a' <= ch && ch <= 'f') digit = ch - 'a' + 10;
    return digit;
}

Bytes readHex(const String &hex)
{
    String s = hex;

    long l = s.startsWith("0x") ? 2 : 0;
    long n = (hex.count() - l) >> 1;

    Bytes data;
    if (n <= 0) return data;

    data = Bytes::allocate(n);
    for (long i = 0; i < n; ++i) {
        data[i] = (readHexDigit(hex[l]) << 4) | readHexDigit(hex[l + 1]);
        l += 2;
    }

    return data;
}

String fnum(double x, int precision, int base, int screen)
{
    /// \todo make use of frexp

    using std::log;
    using std::pow;

    static_assert(sizeof(double) == 8);

    std::uint64_t xi = union_cast<std::uint64_t>(x);
    std::uint64_t f = (xi << 12) >> 12; // fraction
    int e = int((xi << 1) >> 53); // exponent
    int s = int(xi >> 63); // sign

    String text;
    int i = 0;

    if ((e == 0x7FF) && (f != 0)) // NaN
    {
        return "nan";
    }
    else if ((e == 0x7FF) && (f == 0)) // infinite
    {
        return s ? "-inf" : "inf";
    }
    else if ((e == 0) && (f == 0)) // zero
    {
        return "0";
    }
    else // if (((0 < e) && (e < 0x7FF)) || ((e == 0) && (f != 0))) // normalized or denormalized number
    {
        Queue<char> digits;

        bool normalized = (0 < e) && (e < 0x7FF);
        int eb = e - 1023 + int(!normalized); // exponent with bias applied

        int eba = int(roundPoor(log(pow(double(2.), eb)) / log(double(base)))); // exponent in user defined base
        double cba = pow(double(2.),double(eb)) / pow(double(base),double(eba)); // correction factor of user defined base

        uint64_t m = (uint64_t(normalized) << 52) | f; // mantissa
        const uint64_t q = uint64_t((uint64_t(1) << 52) / cba); // quotient

        // workaround to prevent leading zeros
        if (m < q) {
            m *= base;
            --eba;
        }

        int ni = 1; // number of digits of integral part
        if ((-screen <= eba) && (eba <= screen))
        {
            if (eba > 0) {
                ni += eba;
                eba = 0;
            }
            else {
                while (eba < 0) {
                    digits << 0;
                    ++eba;
                }
            }
        }

        while (digits.count() < precision) {
            int d = int(m / q);
            digits << d;
            m -= d * q;
            m *= base;
        }

        int ns = 0; // number of significiant digits
        for (auto pos = digits.head(); pos; ++pos) {
            if (digits.at(pos) != 0) ns = +pos + 1;
        }

        int wi = ni + s;
        int wf = (ns > ni) ? ns - ni : 0;

        int ne = 0; // number of exponent digits
        {
            for (int h = eba; h != 0; h /= base) ++ne;
        }

        text = String::allocate(wi + int(wf != 0) + wf + int(ne != 0) * (1 + int(eba < 0) + ne), ' ');

        if (s == 1)
            text.at(i++) = '-';

        const char *fig = "0123456789abcdef";

        auto di = digits.head(); // digit iterator

        for (int l = 0; l < ni; ++l)
            text.at(i++) = fig[int(digits.at(di++))];

        if (wf != 0)
        {
            text.at(i++) = '.';
            for (int l = 0; l < wf; ++l)
            {
                if (!di)
                    text.at(i++) = '0';
                else
                    text.at(i++) = fig[int(digits.at(di++))];
            }
        }

        if (ne != 0)
        {
            text.at(i++) = 'e';
            if (eba < 0) { text.at(i++) = '-'; eba = -eba; }
            for (int l = ne-1, h = eba; l >= 0; --l, h /= base)
                text.at(i+l) = fig[h % base];
            i += ne;
        }
    }

    return text;
}

String fixed(double x, int nf)
{
    return fixed(x, 0, nf);
}

String fixed(double x, int ni, int nf)
{
    if (x != x) return "nan";
    if (x == +1.0/0.0) return "inf";
    if (x == -1.0/0.0) return "-inf";

    double ip;
    double fp = std::modf(x, &ip);
    if (fp < 0) fp = -fp;

    {
        double q = 1;
        for (int i = 0; i < nf; ++i) q *= 10;
        fp = std::round(q * fp);
        if (fp >= q) {
            if (ip < 0) --ip;
            else ++ip;
            fp -= q;
        }
    }

    String sip = inum(std::int64_t(ip));
    if (ni > sip.count()) sip = sip.alignedRight(ni);
    if (nf <= 0) return sip;

    String s = String::allocate(sip.count() + 1 + nf, '.');
    sip.copyTo(&s);
    inum(std::uint64_t(fp)).alignedRight(nf, '0').copyToRange(&s, sip.count() + 1, s.count());
    return s;
}

String str(const void *x)
{
    if (sizeof(void *) == 8)
        return dec(union_cast<std::uint64_t>(x));

    return dec(union_cast<std::uint32_t>(x));
}

String str(char32_t ch)
{
    String s = String::allocate(4);
    Utf8Sink sink{s};
    sink.write(ch);
    s.truncate(sink.currentOffset());
    return s;
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SystemError>
#include <cc/str>

namespace cc {

String systemError(int errorCode)
{
#ifdef __USE_GNU
    static thread_local String buffer = String::allocate(256);
    const char *unknown = "Unknown error";
    std::memcpy(buffer.items(), unknown, std::strlen(unknown) + 1);
    return strerror_r(errorCode, buffer, buffer.count());
#else
    static thread_local String buffer = String::allocate(256, '\0');
    /*int ret = */strerror_r(errorCode, buffer, buffer.count());
    return buffer;
#endif
}

String SystemError::message() const
{
    return systemError(errorCode_);
}

String SystemResourceError::message() const
{
    return List<String>{} << systemError(errorCode_) << ": \"" << resource_ << "\""
        #ifndef NDEBUG
        << " (" << String{source_}.fileName() << ":" << str(line_) << ", errno = " << str(errorCode_) << ")"
        #endif
        ;
}

String SystemDebugError::message() const
{
    return List<String>{} << systemError(errorCode_) << " (" << String{source_}.fileName() << ":" << str(line_) << ", errno = " << str(errorCode_) << ")";
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PluginLoader>
#include <cc/PluginDir>

namespace cc {

PluginLoader::PluginLoader(const String &libraryPath, const String &group)
{
    String libraryName = libraryPath.baseName();
    String libraryInstallPath = libraryPath.cdUp();
    String pluginPath = libraryPath.cdUp() / libraryName / "plugins";
    if (group != "") pluginPath = pluginPath / group;
    PluginDir{pluginPath};
}

PluginLoader::PluginLoader(const String &libraryPath, const List<String> groups)
{
    for (const String &group: groups) {
        PluginLoader{libraryPath, group};
    }
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CaptureSink>

namespace cc {

struct CaptureSink::State: public Stream::State
{
    long read(Out<Bytes> buffer, long maxFill) override
    {
        return 0;
    }

    void write(const Bytes &buffer, long fill) override
    {
        parts.append(buffer.copy(0, fill > 0 ? fill : buffer.count()));
    }

    List<Bytes> parts;
};

CaptureSink::CaptureSink():
    Stream{new CaptureSink::State}
{}

List<Bytes> CaptureSink::collect() const
{
    return me().parts;
}

Bytes CaptureSink::writtenData() const
{
    List<Bytes> parts = me().parts;
    return (parts.count() == 1) ? parts.at(0) : parts.join();
}

const CaptureSink::State &CaptureSink::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/UserInfo>
#include <sys/types.h>
#include <unistd.h> // sysconf
#include <pwd.h>

namespace cc {

UserInfo::State::State(const struct passwd &info):
    id{static_cast<int>(info.pw_uid)},
    groupId{static_cast<int>(info.pw_gid)},
    loginName{info.pw_name},
    fullName{info.pw_gecos},
    home{info.pw_dir},
    shell{info.pw_shell}
{
    if (fullName.startsWith(',') || fullName.endsWith(',')) {
        fullName = loginName.copy();
        fullName[0] = toUpper(fullName[0]);
    }
}

UserInfo::UserInfo(int id)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct passwd space;
    struct passwd *info = nullptr;
    int ret = ::getpwuid_r(id, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    if (info) Object::me = new State{*info};
}

UserInfo::UserInfo(const String &name)
{
    int bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    String buf = String::allocate(bufSize);
    struct passwd space;
    struct passwd *info = nullptr;
    int ret = ::getpwnam_r(name, &space, buf, +buf, &info);
    if (ret != 0) CC_SYSTEM_RESOURCE_ERROR(ret, name);
    if (info) Object::me = new State{*info};
}

} // namespace cc
/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/bst/OrdinalTree>

namespace cc::bst {

long OrdinalTree::getIndexOf(OrdinalNode *k) const
{
    long i = weight(k->left_);
    while (true) {
        OrdinalNode *kp = k->parent_;
        if (!kp) break;
        if (k == kp->right_)
            i += weight(kp->left_) + 1;
        k = kp;
    }
    assert(k == root_);
    return i;
}

void OrdinalTree::populate(OrdinalNode **v, long n)
{
    for (long i = 0, m = 1; i < n; m *= 2)
    {
        for (long i1 = i + m; i < i1; ++i)
        {
            if (i < n) {
                OrdinalNode *k = v[i];
                OrdinalNode *kp = nullptr;
                if (i > 0) kp = v[(i - 1) >> 1];
                k->parent_ = kp;
                if (kp) {
                    if (i & 1)
                        kp->left_ = k;
                    else
                        kp->right_ = k;
                }
                k->left_ = nullptr;
                k->right_ = nullptr;
            }
            else {
                restoreBalance(v[(i - 1) >> 1], i & 1, false);
            }
        }
    }

    for (long i = n - 1; i > 0; --i) {
        OrdinalNode *k = v[i];
        OrdinalNode *kp = k->parent_;
        kp->weight_ += k->weight_;
    }

    root_ = v[0];

    /*
    assert(testStructure(root_));
    assert(testIteration(root_));
    assert(testBalance(root_));
    assert(testWeight(root_));*/
}

OrdinalNode *OrdinalTree::getNodeAt(long i) const
{
    assert(0 <= i && i < weight());

    const bool cacheAccess = (owner_ == std::this_thread::get_id());
    if (cacheAccess && cachedNode_) {
        const long d = i - cachedIndex_;
        if (d == 1) {
            ++cachedIndex_;
            cachedNode_ = succ(cachedNode_);
            return cachedNode_;
        }
        else if (d == 0) {
            return cachedNode_;
        }
        else if (d == -1) {
            --cachedIndex_;
            cachedNode_ = pred(cachedNode_);
            return cachedNode_;
        }
    }

    OrdinalNode *k = root_;
    long j0 = 0;
    while (k) {
        long j = j0 + weight(k->left_);
        if (i < j) {
            k = k->left_;
        }
        else if (j < i) {
            j0 = j + 1;
            k = k->right_;
        }
        else // i == j
            break;
    }

    if (cacheAccess) {
        cachedNode_ = k;
        cachedIndex_ = i;
    }

    return k;
}

void OrdinalTree::attachBefore(OrdinalNode *kb, OrdinalNode *kn)
{
    if (!kb)
        OrdinalTree::attach(kb, kn, true);
    else if (kb->left_)
        OrdinalTree::attach(max(kb->left_), kn, false);
    else
        OrdinalTree::attach(kb, kn, true);
}

#if 0
void OrdinalTree::attachAfter(OrdinalNode *ka, OrdinalNode *kn)
{
    if (!ka)
        OrdinalTree::attach(ka, kn, true);
    else if (ka->right_)
        OrdinalTree::attach(min(ka->right_), kn, true);
    else
        OrdinalTree::attach(ka, kn, false);
}
#endif

void OrdinalTree::attachAt(long i, OrdinalNode *kn)
{
    if (i == weight()) {
        OrdinalNode *kp = nullptr;
        if (cachedNode_) {
            if (cachedIndex_ == i)
                kp = cachedNode_;
        }
        if (!kp) kp = max();
        attach(kp, kn, false);
    }
    else {
        OrdinalNode *ka = getNodeAt(i);
        assert(ka);
        attachBefore(ka, kn);
    }

    cachedNode_ = kn;
    cachedIndex_ = i;
}

OrdinalNode *OrdinalTree::unlinkAt(long i)
{
    OrdinalNode *ko = getNodeAt(i);
    assert(ko);
    OrdinalNode *k = pred(ko);
    if (k) --i;
    else k = succ(ko);
    unlink(ko);
    if (k) {
        cachedNode_ = k;
        cachedIndex_ = i;
    }
    return ko;
}

void OrdinalTree::changed(OrdinalNode *kp, OrdinalNode *kc, bool left, bool attached)
{
    long delta = attached ? 1 : -1;
    OrdinalNode *k = kp;
    while (k) {
        k->weight_ += delta;
        k = k->parent_;
    }

    restoreBalance(kp, left, attached);

    cachedNode_ = nullptr;
}

void OrdinalTree::rotated(OrdinalNode *k1, bool /*left*/)
{
    establishWeight(k1);
    establishWeight(k1->parent_);
}

#ifndef NDEBUG

bool OrdinalTree::testWeight(OrdinalNode *k)
{
    if (!k) return true;
    return
        (weight(k->left_) + weight(k->right_) + 1 == k->weight_) &&
        testWeight(k->left_) && testWeight(k->right_);
}

#endif // ndef NDEBUG

} // namespace cc::bst
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/System>
#include <unistd.h> // sysconf(3)
#ifdef __OpenBSD__
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#endif
#include <time.h>

namespace cc {

int System::concurrency()
{
    #ifdef _SC_NPROCESSORS_ONLN
        return sysconf(_SC_NPROCESSORS_ONLN);
    #endif

    #ifdef _SC_NPROC_ONLN
        return sysconf(_SC_NPROC_ONLN);
    #endif

    #ifndef __linux
    #ifdef HW_NCPU
    int n = 1;
    size_t sz = sizeof(n);
    int mib[4];
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    sysctl(mib, 2, &n, &sz, NULL, 0);
    if(n > 1) return n;
    #endif
    #endif

    return 1;
}

int System::pageSize()
{
    return sysconf(_SC_PAGE_SIZE);
}

double System::now()
{
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    return double(ts.tv_sec) + double(ts.tv_nsec) / 1e9;
}

String System::hostName()
{
    const int bufSize = 256;
    char buf[bufSize + 1];
    if (::gethostname(buf, bufSize) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    buf[bufSize] = 0;
    return buf;
}

void System::setHostName(const String &newName)
{
    if (::sethostname(newName, newName.count()) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LineBuffer>

namespace cc {

LineBuffer::State::State(const Stream &sink, const String &prefix):
    sink_{sink},
    prefix_{prefix}
{}

long LineBuffer::State::read(Out<Bytes> buffer, long maxFill)
{
    return (sink_) ? sink_.read(buffer, maxFill) : 0;
}

void LineBuffer::State::write(const Bytes &buffer, long fill)
{
    long i = 0, n = (fill < 0 || buffer.count() < fill) ? buffer.count() : fill;

    while (i < n) {
        long i0 = i;
        if (buffer.find('\n', &i)) {
            ++i;
            backlog_.pushBack(buffer.copy(i0, i));
            i0 = i;
            flush();
        }
        else {
            backlog_.pushBack(buffer.copy(i0, n));
        }
    }
}

String LineBuffer::State::prefix() const
{
    return prefix_;
}

long LineBuffer::State::flush()
{
    String h = prefix();
    if (backlog_.count() == 0) return 0;
    if (h != "") backlog_.pushFront(h);
    Bytes line{backlog_};
    writeLine(line);
    backlog_.deplete();
    return line.count();
}

void LineBuffer::State::writeLine(const Bytes &line)
{
    if (sink_) sink_.write(line);
}

LineBuffer::LineBuffer(const Stream &sink, const String &prefix):
    Stream{new LineBuffer::State{sink, prefix}}
{}

LineBuffer::LineBuffer(State *newState):
    Stream{newState}
{}

long LineBuffer::flush()
{
    return me().flush();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Plugin>
#include <dlfcn.h>

namespace cc {

Plugin::Plugin(const String &path):
    Object{new State{path, path.baseName()}}
{
    load();
}

void Plugin::load()
{
    me().handle_ = dlopen(me().path_, RTLD_LAZY|RTLD_LOCAL);
    if (!me().handle_) throw PluginError{};
}

void Plugin::reload()
{
    if (dlclose(me().handle_) != 0) throw PluginError{};
    load();
}

void Plugin::close()
{
    if (me().handle_) {
        dlclose(me().handle_);
        me().handle_ = nullptr;
    }
}

String PluginError::message() const
{
    return dlerror();
}

} // namespace cc
/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Exception>

namespace cc {

const char* Exception::what() const throw()
{
    static thread_local String m = message();
    return m;
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TransferMeter>

namespace cc {

struct TransferMeter::State: public Stream::State
{
    State(const Stream &stream):
        stream_{stream}
    {}

    bool waitEstablished(int timeout) override
    {
        return stream_.waitEstablished(timeout);
    }

    bool wait(IoEvent event, int timeout) override
    {
        return stream_.wait(event, timeout);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        long n = stream_.read(buffer, maxFill);
        totalRead_ += n;
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        totalWritten_ += fill > 0 ? fill : buffer.count();
        stream_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        for (const Bytes &buffer: buffers) {
            totalWritten_ += buffer.count();
        }
        stream_.write(buffers);
    }

    Stream stream_;
    long long totalRead_ { 0 };
    long long totalWritten_ { 0 };
};

TransferMeter::TransferMeter(const Stream &stream):
    Stream{new State{stream}}
{}

long long TransferMeter::totalRead() const
{
    return me().totalRead_;
}

long long TransferMeter::totalWritten() const
{
    return me().totalWritten_;
}

const TransferMeter::State &TransferMeter::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SpinLock>
#include <sched.h>
#ifdef CC_VALGRIND
#include <valgrind/helgrind.h>
#endif

namespace cc {

#ifdef CC_VALGRIND

SpinLock::SpinLock():
    flag_{0}
{
    VALGRIND_HG_MUTEX_INIT_POST(&flag_, 0);
}

SpinLock::~SpinLock()
{
    VALGRIND_HG_MUTEX_DESTROY_PRE(&flag_);
}

bool SpinLock::tryAcquire()
{
    VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 1);
    bool gotIt = !flag_.test_and_set(std::memory_order_acquire);
    VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
    return gotIt;
}

void SpinLock::acquire()
{
    VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 0);
    while (flag_.test_and_set(std::memory_order_acquire)) yield();
    VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
}

void SpinLock::release()
{
    VALGRIND_HG_MUTEX_UNLOCK_PRE(&flag_);
    flag_.clear(std::memory_order_release);
    VALGRIND_HG_MUTEX_UNLOCK_POST(&flag_);
}

#else

SpinLock::SpinLock():
    flag_{0}
{}

SpinLock::~SpinLock()
{}

bool SpinLock::tryAcquire()
{
    return !flag_.test_and_set(std::memory_order_acquire);
}

void SpinLock::acquire()
{
    while (flag_.test_and_set(std::memory_order_acquire)) yield();
}

void SpinLock::release()
{
    flag_.clear(std::memory_order_release);
}
#endif // def CC_VALGRIND

void SpinLock::yield()
{
    sched_yield();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ReadWriteLock>

namespace cc {

ReadWriteLock::ReadWriteLock()
{
    int ret = pthread_rwlock_init(&lock_, nullptr);
    if (ret != 0) throw SystemError{ret};
}

ReadWriteLock::~ReadWriteLock()
{
    pthread_rwlock_destroy(&lock_);
}

void ReadWriteLock::readAcquire()
{
    int ret = pthread_rwlock_rdlock(&lock_);
    if (ret != 0) throw SystemError{ret};
}

void ReadWriteLock::writeAcquire()
{
    int ret = pthread_rwlock_wrlock(&lock_);
    if (ret != 0) throw SystemError{ret};
}

void ReadWriteLock::release()
{
    int ret = pthread_rwlock_unlock(&lock_);
    if (ret != 0) throw SystemError{ret};
}

} // namespace cc
