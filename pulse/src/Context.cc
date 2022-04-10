/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/pulse/Context>

namespace cc::pulse {

struct Context::State: public Object::State
{
    State(const MainLoop &mainLoop, const String &appName):
        mainLoop_{mainLoop},
        appName_{appName},
        context_{pa_context_new(mainLoop_.api(), appName_)}
    {
        pa_context_set_state_callback(context_, &stateChanged, this);
    }

    ~State()
    {
        pa_context_unref(context_);
    }

    static void stateChanged(pa_context *context, void *userData)
    {
        static_cast<State *>(userData)->stateChanged(pa_context_get_state(context));
    }

    void stateChanged(pa_context_state_t newState)
    {
        switch (newState)
        {
            case PA_CONTEXT_UNCONNECTED:
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;

            case PA_CONTEXT_READY:
                onReady_();
                break;
                /*pa_operation_unref(pa_context_get_server_info(
                    c, pulseaudio_server_info_callback, userdata));
                break;
            }*/

            case PA_CONTEXT_FAILED:
            case PA_CONTEXT_TERMINATED:
                mainLoop_.quit(0);
                break;
        };
    }

    static void serverInfoReceived(pa_context *context, const pa_server_info *serverInfo, void *userData)
    {
        static_cast<State *>(userData)->serverInfoReceived_(ServerInfo{serverInfo});
    }

    void requestServerInfo(Call<void(const ServerInfo &)> &&handler)
    {
        serverInfoReceived_ = std::move(handler);

        pa_operation_unref(
            pa_context_get_server_info(context_, &serverInfoReceived, this)
        );
    }

    void connect(Call<void()> &&ready)
    {
        CC_PULSE(pa_context_connect(context_, nullptr, PA_CONTEXT_NOFLAGS, nullptr));
        onReady_ = std::move(ready);
    }

    MainLoop mainLoop_;
    String appName_;
    pa_context *context_ { nullptr };

    Call<void()> onReady_;
    Call<void(const ServerInfo &)> serverInfoReceived_;
};

Context::Context(const MainLoop &mainLoop, const String &appName):
    Object{new State{mainLoop, appName}}
{}

void Context::connect(Call<void()> &&ready)
{
    me().connect(std::move(ready));
}

void Context::requestServerInfo(Call<void(const ServerInfo &)> &&handler)
{
    me().requestServerInfo(std::move(handler));
}

Context::operator pa_context *()
{
    return me().context_;
}

Context::State &Context::me()
{
    return Object::me.as<State>();
}

const Context::State &Context::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::pulse