/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_LOG_H
#define FLUXNODE_LOG_H

#include <flux/Date>
#include <flux/Stream>
#include <flux/Mutex>
#include "LogConfig.h"

namespace fluxnode {

using namespace flux;

class LogMaster;

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
    Ref<LogMaster> logMaster_;
    Ref<Mutex> mutex_;
    Ref<LogConfig> config_;
    Ref<Stream> errorStream_;
    Ref<Stream> warningStream_;
    Ref<Stream> noticeStream_;
    Ref<Stream> infoStream_;
    Ref<Stream> debugStream_;
};

} // namespace fluxnode

#endif // FLUXNODE_LOG_H
