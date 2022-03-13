/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/pulse/InputStream>
#include <cc/Format>
#include <cc/DEBUG>
#include <pulse/stream.h>
#include <cassert>

namespace cc::pulse {

struct InputStream::State: public Object::State
{
    State(const Context &context, const String &name, int sampleRate, int channelCount):
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

    void connect(const String &target, Call<void()> &&ready)
    {
        if (ready) {
            onReady_ = std::move(ready);
        }

        const char *device = nullptr;
        if (target.count() > 0) device = target;

        CC_PULSE(
            pa_stream_connect_record(stream_, device, nullptr, PA_STREAM_NOFLAGS)
        );
    }

    Context context_;
    pa_stream *stream_ { nullptr };
    pa_sample_spec sampleSpec_ {};
    String name_;
    Call<void()> onReady_;
    Call<void(const Bytes &data)> sample_;
    Bytes wrapping_;
};

InputStream::InputStream(const Context &context, int sampleRate, int channelCount):
    Object{new State{context, {}, sampleRate, channelCount}}
{}

int InputStream::sampleRate() const
{
    return me().sampleSpec_.rate;
}

void InputStream::incoming(Call<void(const Bytes &data)> &&sample)
{
    me().sample_ = std::move(sample);
}

void InputStream::connect(const String &target, Call<void()> &&ready)
{
    me().connect(target, std::move(ready));
}

InputStream::State &InputStream::me()
{
    return Object::me.as<State>();
}

const InputStream::State &InputStream::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::pulse
