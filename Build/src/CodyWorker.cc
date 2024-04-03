/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyWorker>
#include <cc/build/CodyTransport>
#include <cc/build/ImportManager>
#include <cc/Thread>
#include <cc/Format>
#include <cc/File>
#include <cc/debugging>

namespace cc::build {

struct CodyWorker::State final: public Object::State
{
    explicit State(
        const Stream &stream,
        const String &cachePrefix,
        Function<bool(const String &, Out<String>, Out<bool>)> &&onIncludeTranslate,
        Function<String(const String &)> &&onModuleExport,
        Function<void(const String &, const List<String> &, const List<String> &)> &&onModuleCompiled,
        Function<bool(const String &, Out<String>, Out<bool>, Out<bool>)> &&onModuleImport,
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
                        String reply = "HELLO 1 ccbuild 0"; // FIXME: omit "0" for efficiency
                        cody.write(reply);
                        established_ = true;
                        target_ = message(3);
                    }
                    continue;
                }
                if (message(0) == "MODULE-REPO") {
                    String reply = Format{"PATHNAME '%%'"} << cachePrefix_;
                    cody.write(reply);
                }
                else if (message(0) == "MODULE-EXPORT") {
                    assert(module_ == "");
                    module_ = message(1);
                    String cachePath = onModuleExport_(module_);
                    String reply = Format{"PATHNAME '%%'"} << cachePath;
                    cody.write(reply);
                }
                else if (message(0) == "MODULE-COMPILED") {
                    const String module = message(1);
                    onModuleCompiled_(module, includes_, imports_);
                    const String reply = "OK";
                    cody.write(reply);
                }
                else if (message(0) == "MODULE-IMPORT") {
                    const String import = message(1);
                    String cachePath;
                    String reply;
                    bool inScope = true;
                    bool isHeader = false;
                    if (onModuleImport_(import, &cachePath, &inScope, &isHeader)) {
                        reply = Format{"PATHNAME '%%'"} << cachePath;
                    }
                    else {
                        reply = Format{"ERROR 'Import of module \'%%\' failed'"} << import;
                    }
                    cody.write(reply);
                    if (inScope) {
                        if (isHeader)
                            includes_.append(import);
                        else
                            imports_.append(import);
                    }
                }
                else if (message(0) == "INCLUDE-TRANSLATE") {
                    String include = message(1);
                    String reply;
                    String cachePath;
                    bool inScope = true;
                    if (onIncludeTranslate_(include, &cachePath, &inScope)) {
                        if (cachePath != "")
                            reply = Format{"PATHNAME '%%'"} << cachePath;
                        else
                            reply = "BOOL TRUE";
                    }
                    else {
                        reply = "BOOL FALSE";
                    }
                    cody.write(reply);
                    if (inScope) includes_.append(include);
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

            if (target_ != "") {
                const String path = target_.sansFileSuffix();
                File::save(path + ".include", includes_.join('\n'));
                File::save(path + ".import", imports_.join('\n'));
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
    String target_;
    String module_;
    List<String> includes_;
    List<String> imports_;
    ImportManager importManager_;
    Function<bool(const String &, Out<String>, Out<bool>)> onIncludeTranslate_;
    Function<String(const String &)> onModuleExport_;
    Function<void(const String &, const List<String> &, const List<String> &)> onModuleCompiled_;
    Function<bool(const String &, Out<String>, Out<bool>, Out<bool>)> onModuleImport_;
    Function<int(const List<String> &)> onInvoke_;
};

CodyWorker::CodyWorker(
    const Stream &stream,
    const String &cachePrefix,
    Function<bool(const String &, Out<String>, Out<bool>)>  &&onIncludeTranslate,
    Function<String(const String &)> &&onModuleExport,
    Function<void(const String &, const List<String> &, const List<String> &)> &&onModuleCompiled,
    Function<bool(const String &, Out<String>, Out<bool>, Out<bool>)> &&onModuleImport,
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
