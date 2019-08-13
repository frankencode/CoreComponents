/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include "LogConfig.h"

namespace ccnode {

using namespace cc;

class Log: public Object
{
public:
    void open(const LogConfig *config);

    String path() const { return config_->path(); }
    LogLevel level() const { return config_->level(); }
    double retentionPeriod() const { return config_->retentionPeriod(); }
    double rotationInterval() const { return config_->rotationInterval(); }

    Stream *errorStream() const { return errorStream_; }
    Stream *warningStream() const { return warningStream_; }
    Stream *noticeStream() const { return noticeStream_; }
    Stream *infoStream() const { return infoStream_; }
    Stream *debugStream() const { return debugStream_; }

protected:
    Log();
    ~Log();

private:
    Ref<const LogConfig> config_;
    Ref<Stream> errorStream_;
    Ref<Stream> warningStream_;
    Ref<Stream> noticeStream_;
    Ref<Stream> infoStream_;
    Ref<Stream> debugStream_;
};

} // namespace ccnode
