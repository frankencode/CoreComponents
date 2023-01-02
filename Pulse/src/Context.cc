/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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

    void requestServerInfo(Function<void(const ServerInfo &)> &&handler)
    {
        serverInfoReceived_ = move(handler);

        pa_operation_unref(
            pa_context_get_server_info(context_, &serverInfoReceived, this)
        );
    }

    void connect(Function<void()> &&ready)
    {
        CC_PULSE(pa_context_connect(context_, nullptr, PA_CONTEXT_NOFLAGS, nullptr));
        onReady_ = move(ready);
    }

    MainLoop mainLoop_;
    String appName_;
    pa_context *context_ { nullptr };

    Function<void()> onReady_;
    Function<void(const ServerInfo &)> serverInfoReceived_;
};

Context::Context(const MainLoop &mainLoop, const String &appName):
    Object{new State{mainLoop, appName}}
{}

void Context::connect(Function<void()> &&ready)
{
    me().connect(move(ready));
}

void Context::requestServerInfo(Function<void(const ServerInfo &)> &&handler)
{
    me().requestServerInfo(move(handler));
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
