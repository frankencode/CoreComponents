/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyWorker>
#include <cc/build/CodyMessageSource>
#include <cc/Thread>
#include <cc/Format>
#include <cc/DEBUG>

namespace cc::build {

struct CodyWorker::State final: public Object::State
{
    explicit State(
        const Stream &stream,
        const String &cachePrefix,
        Function<String(const String &)> &&onModuleExport,
        Function<void(const String &)> &&onModuleCompiled,
        Function<bool(const String &, const String &, Out<String>)> &&onModuleImport,
        Function<int(const List<String> &)> &&onInvoke
    ):
        stream_{stream},
        cachePrefix_{cachePrefix},
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
            CodyMessageSource source { stream_ };
            for (CodyMessage message; source.read(&message);) {
                if (message.count() == 0) continue;
                CC_INSPECT(message.toString());
                if (!established_) {
                    if (message(0) == "HELLO") {
                        if (!message(1).read<int>(&compilerProtocolVersion_)) {
                            throw CodyError { Format{"Expected protocol version number after HELLO: \"%%\""} << message.toString() };
                        }
                        if (compilerProtocolVersion_ != 1) {
                            stream_.write("ERROR Unsupported protocol version\n");
                            throw CodyError{Format{"Unsupported protocol version (expected 1): \"%%\""} << message.toString()};
                        }
                        String reply = "HELLO 1 ccbuild 0\n";
                        CC_INSPECT(reply);
                        stream_.write(reply);
                        established_ = true;
                    }
                    continue;
                }
                if (message(0) == "MODULE-REPO") {
                    // String reply = Format{"PATHNAME '%%'\n"} << cachePrefix_;
                    String reply { "PATHNAME ''\n" };
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "MODULE-EXPORT") {
                    module_ = message(1);
                    String cachePath = onModuleExport_(message(1));
                    String reply = Format{"PATHNAME '%%'\n"} << cachePath;
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "MODULE-COMPILED") {
                    onModuleCompiled_(message(1));
                    String reply = "OK\n";
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "MODULE-IMPORT") {
                    String importModule = message(1);
                    String importCachePath;
                    String reply;
                    if (onModuleImport_(module_, importModule, &importCachePath)) {
                        reply = Format{"PATHNAME '%%'\n"} << importCachePath;
                    }
                    else {
                        reply = Format{"ERROR 'Import of module \'%%\' failed'\n"} << importModule;
                    }
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "INCLUDE-TRANSLATE") {
                    String reply { "BOOL TRUE\n" };
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "INVOKE") {
                    message.words().popFront();
                    int ret = onInvoke_(message.words());
                    String reply;
                    if (ret == 0) {
                        reply = "OK\n";
                    }
                    else {
                        reply = Format{"ERROR %%\n"} << ret;
                    }
                    CC_INSPECT(reply);
                    stream_.write(reply);
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
    Function<String(const String &)> onModuleExport_;
    Function<void(const String &)> onModuleCompiled_;
    Function<bool(const String &, const String &, Out<String>)> onModuleImport_;
    Function<int(const List<String> &)> onInvoke_;
};

CodyWorker::CodyWorker(
    const Stream &stream,
    const String &cachePrefix,
    Function<String(const String &)> &&onModuleExport,
    Function<void(const String &)> &&onModuleCompiled,
    Function<bool(const String &, const String &, Out<String>)> &&onModuleImport,
    Function<int(const List<String> &)> &&onInvoke
):
    Object{
        new State{
            stream,
            cachePrefix,
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
