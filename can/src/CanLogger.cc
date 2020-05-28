/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanLogger>
#include <cc/can/HeartBeat>
#include <cc/can/Emergency>
#include <cc/can/NetworkCommand>
#include <cc/can/CanError>
#include <cc/can/ServiceRequest>
#include <cc/can/ServiceReply>
#include <cc/can/AbortRequest>
#include <cc/can/AbortReply>
#include <cc/can/BlockReadStartRequest>
#include <cc/can/BlockWriteInitReply>
#include <cc/can/BlockWriteSegmentRequest>
#include <cc/can/BlockReadSegmentReply>
#include <cc/can/SaeMessage>
#include <cc/stdio>
#include <cc/Worker>
#include <cc/System>
#include <cc/Set>

namespace cc {
namespace can {

Ref<CanLogger> CanLogger::create(CanMedia *media)
{
    Ref<CanLogger> logger = new CanLogger(media);
    logger->bootstrap();
    return logger;
}

CanLogger::CanLogger(CanMedia *media):
    media_{media}
{}

void CanLogger::bootstrap()
{
    worker_ = Worker::start([=]{
        auto blockWriteTransfers = Set<int>::create();
        auto blockReadTransfers = Set<int>::create();

        typedef Map<int, double> LastTimes;
        auto lastTimes = LastTimes::create();

        double t0 = -1;

        for (auto frame = CanFrame::create(); media_->readFrame(frame);)
        {
            double t = System::now();
            if (t0 < 0) t0 = t;
            t -= t0;

            double dt = 0, tl = 0;
            if (lastTimes->lookup(frame->canId(), &tl))
                dt = t - tl;
            lastTimes->establish(frame->canId(), t);

            Format f;
            f << fixed(t, 3, 3) << " " << fixed(dt, 3, 3) << " -- " << frame << nl;

            String info;

            if (ServiceReply{frame})
            {
                if (BlockWriteInitReply{frame})
                    blockWriteTransfers->insert(BlockWriteInitReply{frame}->serverId());
                else if (AbortReply{frame})
                    blockWriteTransfers->remove(AbortReply{frame}->serverId());

                if (blockReadTransfers->contains(ServiceReply{frame}->serverId())) {
                    if (BlockReadSegmentReply{frame}) {
                        if (BlockReadSegmentReply{frame}->isLast())
                            blockReadTransfers->remove(ServiceReply{frame}->serverId());

                        info = BlockReadSegmentReply{frame};
                    }
                }
            }
            else if (ServiceRequest{frame})
            {
                if (BlockReadStartRequest{frame})
                    blockReadTransfers->insert(BlockReadStartRequest{frame}->serverId());
                else if (AbortRequest{frame})
                    blockReadTransfers->remove(AbortRequest{frame}->serverId());

                if (blockWriteTransfers->contains(ServiceRequest{frame}->serverId())) {
                    if (BlockWriteSegmentRequest{frame}) {
                        if (BlockWriteSegmentRequest{frame}->isLast())
                            blockWriteTransfers->remove(ServiceRequest{frame}->serverId());

                        info = BlockWriteSegmentRequest{frame};
                    }
                }
            }

            if (info == "") {
                if (ServiceRequest{frame})      info = ServiceRequest{frame};
                else if (ServiceReply{frame})   info = ServiceReply{frame};
                else if (HeartBeat{frame})      info = HeartBeat{frame};
                else if (Emergency{frame})      info = Emergency{frame};
                else if (NetworkCommand{frame}) info = NetworkCommand{frame};
                else if (SaeMessage{frame})     info = SaeMessage{frame};
                else if (CanError{frame})       info = CanError{frame};
            }

            if (info != "")
                f << nl << info->indent("  ") << nl << nl;

            stdOut()->write(f);
        }
    });
}

}} // namespace cc::can
