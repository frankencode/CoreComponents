/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobServer>
#include <cc/build/JobState>
#include <cc/build/TerminationReply>

namespace cc::build {

struct JobServer::State: public Object::State
{
    State(const Channel<Message> &requestChannel, const Channel<Message> &replyChannel):
        requestChannel_{requestChannel},
        replyChannel_{replyChannel},
        thread_{[&]{ run(); }}
    {
        thread_.start();
    }

    ~State()
    {
        requestChannel_.shutdown();
        thread_.wait();
    }

    void run()
    {
        for (Message message; requestChannel_.popFront(&message);) {
            if (message.type() == Message::Type::Job) {
                Job job = message.as<Job>();
                job.run();
                replyChannel_.pushBack(job);
            }
            else if (message.type() == Message::Type::TerminationRequest) {
                replyChannel_.pushFront(TerminationReply{Object::alias<JobServer>(this)});
                break;
            }
        }
    }

    Channel<Message> requestChannel_;
    Channel<Message> replyChannel_;
    Thread thread_;
};

JobServer::JobServer(const Channel<Message> &requestChannel, const Channel<Message> &replyChannel):
    Object{new State{requestChannel, replyChannel}}
{}

} // namespace cc::build
