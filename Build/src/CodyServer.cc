/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyServer>
#include <cc/build/CodyWorker>
#include <cc/build/JobScheduler>
#include <cc/build/ToolChain>
#include <cc/build/ImportManager>
#include <cc/ServerSocket>
#include <cc/ClientSocket>
#include <cc/FileInfo>
#include <cc/Format>
#include <cc/Process>
#include <cc/Thread>
#include <cc/SpinLock>
#include <cc/Channel>
#include <cc/Mutex>
#include <cc/Dir>
#include <cc/File>
#include <cc/DEBUG>

namespace cc::build {

struct CodyServer::State final: public Object::State
{
    explicit State(const BuildPlan &plan, const JobScheduler &scheduler):
        plan_{plan},
        scheduler_{scheduler}
    {
        connectionInfo_ = Format{"localhost:%%"} << listenSocket_.address().port();
        // Process::setEnv("CXX_MODULE_MAPPER", connectionInfo_);
        // CC_INSPECT(Process::env("CXX_MODULE_MAPPER"));
        listenShutdown_.acquire();
    }

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    void shutdown(Out<CodyError> error)
    {
        if (thread_) {
            listenShutdown_.release();
            ClientSocket{listenSocket_.address()}.waitEstablished();
            thread_.wait();
            thread_ = Thread{};

            for (CodyWorker &worker: workers_) {
                CodyError workerError;
                worker.wait(&workerError);
                if (workerError && !error_) error_ = workerError;
            }

            error = error_;
        }
    }

    void run()
    {
        try {
            while (true) {
                StreamSocket stream = listenSocket_.accept();
                if (listenShutdown_.tryAcquire()) {
                    listenShutdown_.release();
                    break;
                }

                CC_INSPECT(stream.address() /*CODY*/);

                workers_.emplaceBack(
                    stream,
                    importManager_.cachePrefix(),
                    [this](const String &include, Out<String> cachePath, Out<bool> inScope) {
                        return onIncludeTranslate(include, &cachePath, &inScope);
                    },
                    [this](const String &module) {
                        return onModuleExport(module);
                    },
                    [this](const String &module, const List<String> &includes, const List<String> &imports){
                        onModuleCompiled(module, includes, imports);
                    },
                    [this](const String &import, Out<String> cachePath, Out<bool> inScope, Out<bool> isHeader){
                        return onModuleImport(import, &cachePath, &inScope, &isHeader);
                    },
                    [this](const List<String> &args){
                        return onInvoke(args);
                    }
                );
            }
        }
        catch (Exception &ex) {
            error_ = CodyError{ex.message()};
        }
    }

    bool onIncludeTranslate(const String &include, Out<String> cachePath, Out<bool> inScope)
    {
        inScope = include.startsWith(plan_.sourcePrefix());
        return true;
    }

    String onModuleExport(const String &module) const
    {
        return importManager_.cachePath(module);
    }

    bool onModuleImport(const String &import, Out<String> cachePath, Out<bool> inScope, Out<bool> isHeader)
    {
        if (isHeaderFile(import)) {
            inScope = import.startsWith(plan_.sourcePrefix());
            isHeader = true;
            return importManager_.compileHeaderUnit(scheduler_, plan_, import, &cachePath);
        }

        String source;
        bool ok = importManager_.compileInterfaceUnit(scheduler_, plan_, import, &cachePath, &source);
        if (ok) inScope = source.startsWith(plan_.sourcePrefix());
        isHeader = false;
        return ok;
    }

    void onModuleCompiled(const String &module, const List<String> &includes, const List<String> &imports)
    {
        const String path = importManager_.cachePrefix() / importManager_.cachePath(module);
        File::save(path + ".include", includes.join('\n'));
        File::save(path + ".import", imports.join('\n'));
    }

    int onInvoke(const List<String> &args)
    {
        Job job { args };
        bool ok = job.run();
        scheduler_.report(job);
        return ok;
    }

    static bool isHeaderFile(const String &source)
    {
        const String ext = source.fileSuffix();

        return
            (ext == "" && source.contains("/include/")) ||
            ext == "h" || ext == "hh" || ext == "hpp" || ext == "hxx" || ext == "h++" || ext == "H";
    }

    BuildPlan plan_;
    JobScheduler scheduler_;
    ImportManager importManager_;

    ServerSocket listenSocket_ { SocketAddress { ProtocolFamily::InternetV6, "::" } };
    String connectionInfo_;
    Thread thread_;
    SpinLock listenShutdown_;
    List<CodyWorker> workers_;

    CodyError error_;
};

CodyServer::CodyServer(const BuildPlan &plan, const JobScheduler &scheduler):
    Object{new State{plan, scheduler}}
{}

String CodyServer::connectionInfo() const
{
    return me().connectionInfo_;
}

void CodyServer::start()
{
    me().start();
}

void CodyServer::shutdown(Out<CodyError> error)
{
    me().shutdown(&error);
}

CodyServer::State &CodyServer::me()
{
    return Object::me.as<State>();
}

const CodyServer::State &CodyServer::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
