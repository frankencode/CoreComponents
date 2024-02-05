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
        const String &cmiCachePath,
        const Channel<String> &modulesCompiled,
        const Channel<CodyImportRequest> &importRequest
    ):
        stream_{stream},
        cmiCachePath_{cmiCachePath},
        modulesCompiled_{modulesCompiled},
        importRequest_{importRequest}
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
                        String reply = Format{"HELLO 1 ccbuild %%\n"} << flags_;
                        CC_INSPECT(reply);
                        stream_.write(reply);
                        established_ = true;
                    }
                    continue;
                }
                if (message(0) == "MODULE-REPO") {
                    String reply = Format{"PATHNAME '%%'\n"} << cmiCachePath_;
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "MODULE-EXPORT") {
                    module_ = message(1);
                    String cmiPath = module_;
                    if (cmiPath.contains('/')) {
                        cmiPath.replace("./", ",/");
                        cmiPath.replace("../", ",,/");
                    }
                    cmiPath += ".gcm";
                    String reply = Format{"PATHNAME '%%'\n"} << cmiPath;
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "MODULE-COMPILED") {
                    modulesCompiled_ << message(1);
                    String reply = "OK\n";
                    CC_INSPECT(reply);
                    stream_.write(reply);
                }
                else if (message(0) == "MODULE-IMPORT") {
                    String module = message(1);
                    CodyImportRequest request { module };
                    importRequest_.pushBack(request);
                    String reply;
                    String cmiPath;
                    if (request.cmiPath().popFront(&cmiPath)) {
                        reply = Format{"PATHNAME '%%'\n"} << cmiPath;
                    }
                    else {
                        reply = Format{"ERROR 'Compilation of module \'%%\' failed'"} << module;
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
    String cmiCachePath_;
    CodyMessage::Flags flags_;
    Thread thread_;
    CodyError error_;
    int compilerProtocolVersion_ { 0 };
    bool established_ { false };
    String module_;
    Channel<String> modulesCompiled_;
    Channel<CodyImportRequest> importRequest_;
};

CodyWorker::CodyWorker(
    const Stream &stream,
    const String &cmiCachePath,
    const Channel<String> &modulesCompiled,
    const Channel<CodyImportRequest> &importRequest
):
    Object{
        new State{
            stream,
            cmiCachePath,
            modulesCompiled,
            importRequest
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
