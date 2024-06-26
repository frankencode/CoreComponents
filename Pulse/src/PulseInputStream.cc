/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PulseInputStream>
#include <cc/Format>
#include <cc/DEBUG>
#include <pulse/stream.h>
#include <cassert>

namespace cc {

struct PulseInputStream::State: public Object::State
{
    State(const PulseContext &context, const String &name, int sampleRate, int channelCount):
        context_{context},
        name_{name}
    {
        if (name_.count() == 0) {
            name_ = Format{"input_%%_%%"} << sampleRate << channelCount;
        }

        sampleSpec_.format = PA_SAMPLE_S16NE;
        sampleSpec_.rate = sampleRate;
        sampleSpec_.channels = channelCount;

        stream_ = pa_stream_new(context_, name_, &sampleSpec_, nullptr);

        pa_stream_set_state_callback(stream_, &stateChanged, this);
        pa_stream_set_read_callback(stream_, &incoming, this);
    }

    ~State()
    {
        if (stream_) {
            pa_stream_unref(stream_);
        }
    }

    static void stateChanged(pa_stream *stream, void *userData)
    {
        static_cast<State *>(userData)->stateChanged(pa_stream_get_state(stream));
    }

    void stateChanged(pa_stream_state newState)
    {
        // CC_INSPECT(newState);
        if (newState == PA_STREAM_READY && onReady_) onReady_();
    }

    static void incoming(pa_stream *stream, size_t fill, void *userData)
    {
        static_cast<State *>(userData)->incoming(fill);
    }

    void incoming(size_t fill)
    {
        if (sample_) {
            const void *data = nullptr;
            CC_PULSE(pa_stream_peek(stream_, &data, &fill));
            if (data != nullptr && fill > 0 && sample_) {
                wrapping_.wrapAround(const_cast<void *>(data), fill);
                sample_(wrapping_);
            }
            if (fill > 0) {
                CC_PULSE(pa_stream_drop(stream_));
            }
        }
    }

    void connect(const String &target, Function<void()> &&ready)
    {
        if (ready) {
            onReady_ = move(ready);
        }

        const char *device = nullptr;
        if (target.count() > 0) device = target;

        CC_PULSE(
            pa_stream_connect_record(stream_, device, nullptr, PA_STREAM_NOFLAGS)
        );
    }

    PulseContext context_;
    pa_stream *stream_ { nullptr };
    pa_sample_spec sampleSpec_ {};
    String name_;
    Function<void()> onReady_;
    Function<void(const Bytes &data)> sample_;
    Bytes wrapping_;
};

PulseInputStream::PulseInputStream(const PulseContext &context, int sampleRate, int channelCount):
    Object{new State{context, {}, sampleRate, channelCount}}
{}

int PulseInputStream::sampleRate() const
{
    return me().sampleSpec_.rate;
}

void PulseInputStream::incoming(Function<void(const Bytes &data)> &&sample)
{
    me().sample_ = move(sample);
}

void PulseInputStream::connect(const String &target, Function<void()> &&ready)
{
    me().connect(target, move(ready));
}

PulseInputStream::State &PulseInputStream::me()
{
    return Object::me.as<State>();
}

const PulseInputStream::State &PulseInputStream::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
