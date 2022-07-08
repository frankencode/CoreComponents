/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FileLoggingService>
#include <cc/FileLoggingInstance>
#include <cc/HttpLoggingServiceRegistry>

namespace cc {

struct FileLoggingService::State: public HttpLoggingService::State
{
    State():
        prototype_{
            []{
                auto prototype = HttpLoggingServiceConfigPrototype{"LogFile"};
                prototype.establish("path", "");
                return prototype;
            }()
        }
    {}

    HttpLoggingServiceConfigPrototype configPrototype() const override
    {
        return prototype_;
    }

    HttpLoggingServiceInstance createInstance(const MetaObject &config) const override
    {
        return FileLoggingInstance{config};
    }

    HttpLoggingServiceConfigPrototype prototype_;
};

FileLoggingService::FileLoggingService():
    HttpLoggingService{new State}
{}

class FileLoggingServiceAnnouncer {
public:
    FileLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        HttpLoggingServiceRegistry{}.registerService(FileLoggingService{});
        done = true;
    }
};

namespace { FileLoggingServiceAnnouncer announcer; }

} // namespace cc
