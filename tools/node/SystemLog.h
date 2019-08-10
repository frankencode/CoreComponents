/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <syslog.h>
#include <cc/Stream>

namespace cc { template<class> class ThreadLocalSingleton; }

namespace ccnode {

using namespace cc;

class SystemLog: public Object
{
public:
    static void open(const String &identifier, int option, int facility);
    static SystemLog *instance();

    Stream *emergencyStream() const { return emergencyStream_; }
    Stream *alertStream()     const { return alertStream_; }
    Stream *criticalStream()  const { return criticalStream_; }
    Stream *errorStream()     const { return errorStream_; }
    Stream *warningStream()   const { return warningStream_; }
    Stream *noticeStream()    const { return noticeStream_; }
    Stream *infoStream()      const { return infoStream_; }
    Stream *debugStream()     const { return debugStream_; }

private:
    friend class ThreadLocalSingleton<SystemLog>;
    SystemLog();

    Ref<Stream> emergencyStream_;
    Ref<Stream> alertStream_;
    Ref<Stream> criticalStream_;
    Ref<Stream> errorStream_;
    Ref<Stream> warningStream_;
    Ref<Stream> noticeStream_;
    Ref<Stream> infoStream_;
    Ref<Stream> debugStream_;
};

} // namespace ccnode
