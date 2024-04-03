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
#include <cc/System>

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

Date Timezone::currentDate()
{
    return date(System::now());
}

double Timezone::nextTime(const Date &date, int hour, int minutes, int seconds)
{
    assert(date.offset() == Timezone::offset(date.time()));

    Date n {
        date.year(),
        date.month(),
        date.day(),
        date.hour(),
        date.minutes(),
        date.seconds(),
        date.offset()
    };

    double t = n.time();
    if (n <= date) t += SecondsPerDay;
    t += n.offset() - Timezone::offset(t);

    return t;
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
