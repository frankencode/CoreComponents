/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyWorker>
#include <cc/build/CodyTransport>
#include <cc/Thread>
#include <cc/Format>

namespace cc::build {

struct CodyWorker::State final: public Object::State
{
    explicit State(
        const Stream &stream,
        const String &cachePrefix,
        Function<bool(const String &, const String &, Out<String>)> &&onIncludeTranslate,
        Function<String(const String &)> &&onModuleExport,
        Function<void(const String &)> &&onModuleCompiled,
        Function<bool(const String &, const String &, Out<String>)> &&onModuleImport,
        Function<int(const List<String> &)> &&onInvoke
    ):
        stream_{stream},
        cachePrefix_{cachePrefix},
        onIncludeTranslate_{onIncludeTranslate},
        onModuleExport_{onModuleExport},
        onModuleCompiled_{onModuleCompiled},
        onModuleImport_{onModuleImport},
        onInvoke_{onInvoke}
    {}

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void wait(Out<CodyError> error)
    {
        thread_.wait();
        error = error_;
    }

    void run()
    {
        try {
            CodyTransport cody { stream_ };
            for (CodyMessage message; cody.read(&message);) {
                if (message.count() == 0) continue;
                if (!established_) {
                    if (message(0) == "HELLO") {
                        if (!message(1).read<int>(&compilerProtocolVersion_)) {
                            throw CodyError { Format{"Expected protocol version number after HELLO: \"%%\""} << message.toString() };
                        }
                        if (compilerProtocolVersion_ != 1) {
                            cody.write("ERROR Unsupported protocol version");
                            throw CodyError{Format{"Unsupported protocol version (expected 1): \"%%\""} << message.toString()};
                        }
                        String reply = "HELLO 1 ccbuild 0";
                        cody.write(reply);
                        established_ = true;
                    }
                    continue;
                }
                if (message(0) == "MODULE-REPO") {
                    String reply = Format{"PATHNAME '%%'"} << cachePrefix_;
                    cody.write(reply);
                }
                else if (message(0) == "MODULE-EXPORT") {
                    module_ = message(1);
                    String cachePath = onModuleExport_(message(1));
                    String reply = Format{"PATHNAME '%%'"} << cachePath;
                    cody.write(reply);
                }
                else if (message(0) == "MODULE-COMPILED") {
                    onModuleCompiled_(message(1));
                    String reply = "OK";
                    cody.write(reply);
                }
                else if (message(0) == "MODULE-IMPORT") {
                    String srcModule = message(1);
                    String cachePath;
                    String reply;
                    if (onModuleImport_(module_, srcModule, &cachePath)) {
                        reply = Format{"PATHNAME '%%'"} << cachePath;
                    }
                    else {
                        reply = Format{"ERROR 'Import of module \'%%\' failed'"} << srcModule;
                    }
                    cody.write(reply);
                }
                else if (message(0) == "INCLUDE-TRANSLATE") {
                    String reply;
                    String cachePath;
                    if (onIncludeTranslate_(module_, message(1), &cachePath)) {
                        if (cachePath != "")
                            reply = Format{"PATHNAME '%%'"} << cachePath;
                        else
                            reply = "BOOL TRUE";
                    }
                    else {
                        reply = "BOOL FALSE";
                    }
                    cody.write(reply);
                }
                else if (message(0) == "INVOKE") {
                    message.words().popFront();
                    int ret = onInvoke_(message.words());
                    String reply;
                    if (ret == 0) {
                        reply = "OK";
                    }
                    else {
                        reply = Format{"ERROR %%"} << ret;
                    }
                    cody.write(reply);
                }
            }
        }
        catch (CodyError &error)
        {
            error_ = error;
        }
        catch (Exception &exception)
        {
            error_ = CodyError { exception.message() };
        }
    }

    Stream stream_;
    String cachePrefix_;
    Thread thread_;
    CodyError error_;
    int compilerProtocolVersion_ { 0 };
    bool established_ { false };
    String module_;
    Function<bool(const String &, const String &, Out<String>)> onIncludeTranslate_;
    Function<String(const String &)> onModuleExport_;
    Function<void(const String &)> onModuleCompiled_;
    Function<bool(const String &, const String &, Out<String>)> onModuleImport_;
    Function<int(const List<String> &)> onInvoke_;
};

CodyWorker::CodyWorker(
    const Stream &stream,
    const String &cachePrefix,
    Function<bool(const String &, const String &, Out<String>)>  &&onIncludeTranslate,
    Function<String(const String &)> &&onModuleExport,
    Function<void(const String &)> &&onModuleCompiled,
    Function<bool(const String &, const String &, Out<String>)> &&onModuleImport,
    Function<int(const List<String> &)> &&onInvoke
):
    Object{
        new State{
            stream,
            cachePrefix,
            std::move(onIncludeTranslate),
            std::move(onModuleExport),
            std::move(onModuleCompiled),
            std::move(onModuleImport),
            std::move(onInvoke)
        }
    }
{
    me().start();
}

void CodyWorker::wait(Out<CodyError> error)
{
    me().wait(&error);
}

CodyWorker::State &CodyWorker::me()
{
    return Object::me.as<State>();
}

const CodyWorker::State &CodyWorker::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
