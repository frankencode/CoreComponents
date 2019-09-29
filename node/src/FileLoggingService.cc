/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/FileLoggingService>
#include <cc/node/FileLoggingInstance>
#include <cc/node/LoggingRegistry>

namespace cc {
namespace node {

Ref<FileLoggingService> FileLoggingService::create()
{
    return new FileLoggingService;
}

FileLoggingService::FileLoggingService():
    prototype_{
        []{
            auto prototype = LoggingPrototype::create("LogFile");
            prototype->establish("path", "");
            return prototype;
        }()
    }
{}

const LoggingPrototype *FileLoggingService::configPrototype() const
{
    return prototype_;
}

Ref<LoggingInstance> FileLoggingService::createInstance(const MetaObject *config) const
{
    return FileLoggingInstance::create(config);
}

class FileLoggingServiceAnnouncer {
public:
    FileLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        LoggingRegistry::instance()->registerService(FileLoggingService::create());
        done = true;
    }
};

namespace { FileLoggingServiceAnnouncer announcer; }

}} // namespace cc::node
