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
#include <cc/ServerSocket>
#include <cc/ClientSocket>
#include <cc/Format>
#include <cc/Process>
#include <cc/Thread>
#include <cc/SpinLock>
#include <cc/Channel>
#include <cc/Mutex>
#include <cc/Dir>
#include <cc/Set>
#include <cc/MultiMap>
#include <cc/DEBUG>

namespace cc::build {

struct CodyServer::State final: public Object::State
{
    explicit State(const JobScheduler &scheduler):
        scheduler_{scheduler}
    {
        Process::setEnv("CXX_MODULE_MAPPER", Format{"localhost:%%"} << listenSocket_.address().port());
        // Process::setEnv("CXX_MODULE_MAPPER", listenSocket_.address().toString());
        CC_INSPECT(listenSocket_.address());
        listenShutdown_.acquire();
        Dir::establish(cachePrefix_);
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
                    cachePrefix_,
                    [this](const String &module) {
                        return onModuleExport(module);
                    },
                    [this](const String &module){
                        onModuleCompiled(module);
                    },
                    [this](const String &module, const String &importModule, Out<String> importCachePath){
                        return onModuleImport(module, importModule, &importCachePath);
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

    static String cachePath(const String &module)
    {
        String cachePath = module;
        if (cachePath.contains('/')) {
            cachePath.replace("./", ",/");
            cachePath.replace("../", ",,/");
        }
        cachePath += ".gcm";
        return cachePath;
    }

    String onModuleExport(const String &module) const
    {
        return cachePath(module);
    }

    bool onModuleImport(const String &module, const String &importModule, Out<String> importCachePath)
    {
        Guard guard {mutex_};
        if (modulesReady_.contains(importModule)) {
            importCachePath = cachePath(module);
            return true;
        }
        Channel<String> waitChannel;
        pendingImportRequests_.insert(module, waitChannel);
        guard.dismiss();
        scheduler_.reportJobSuspension();
        bool ok = waitChannel.popFront(&importCachePath);
        scheduler_.reportJobResumption();
        return ok;
    }

    void onModuleCompiled(const String &module)
    {
        Guard guard{mutex_};
        modulesReady_.insert(module);
        Locator pos;
        if (pendingImportRequests_.find(module, &pos)) {
            do {
                pendingImportRequests_.at(pos).value().pushBack(cachePath(module));
                pendingImportRequests_.removeAt(pos);
            } while (pos && pendingImportRequests_.at(pos).key() == module);
        }
    }

    int onInvoke(const List<String> &args)
    {
        return Process::exec(args);
    }

    const String cachePrefix_ { "gcm.cache" };
    JobScheduler scheduler_;
    ServerSocket listenSocket_ { SocketAddress { ProtocolFamily::InternetV6, "::" } };
    Thread thread_;
    SpinLock listenShutdown_;
    List<CodyWorker> workers_;
    Mutex mutex_;
    Set<String> modulesReady_;
    MultiMap<String, Channel<String> > pendingImportRequests_;

    CodyError error_;
};

CodyServer::CodyServer(const JobScheduler &scheduler):
    Object{new State{scheduler}}
{}

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
