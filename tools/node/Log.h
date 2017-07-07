/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Date>
#include <cc/Stream>
#include <cc/Mutex>
#include "LogConfig.h"

namespace ccnode {

using namespace cc;

enum LogLevel {
    SilentLogLevel  = 0,
    ErrorLogLevel   = 1,
    WarningLogLevel = 2,
    NoticeLogLevel  = 3,
    InfoLogLevel    = 4,
    DebugLogLevel   = 5,
#ifdef NDEBUG
    DefaultLogLevel = NoticeLogLevel
#else
    DefaultLogLevel = DebugLogLevel
#endif
};

class Log: public Object
{
public:
    void open(LogConfig *config);
    void open();

    String path() const { return config_->path(); }
    int level() const { return config_->level(); }
    double retentionPeriod() const { return config_->retentionPeriod(); }
    double rotationInterval() const { return config_->rotationInterval(); }

    Stream *errorStream() const;
    Stream *warningStream() const;
    Stream *noticeStream() const;
    Stream *infoStream() const;
    Stream *debugStream() const;

protected:
    Log();
    ~Log();

private:
    Ref<Mutex> mutex_;
    Ref<LogConfig> config_;
    Ref<Stream> errorStream_;
    Ref<Stream> warningStream_;
    Ref<Stream> noticeStream_;
    Ref<Stream> infoStream_;
    Ref<Stream> debugStream_;
};

} // namespace ccnode
