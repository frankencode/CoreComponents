/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobState>

namespace cc::build {

Job::Job(const String &command):
    Message{new State{command}}
{}

Job::Job(State *newState):
    Message{newState}
{}

String Job::command() const
{
    return me().command_;
}

int Job::status() const
{
    return me().status_;
}

String Job::outputText() const
{
    return me().outputText_;
}

void Job::registerDerivative(Job &job)
{
    me().derivatives_.pushBack(job);
    ++job.me().countDown_;
}

bool Job::getNextDerivative(Out<Job> derivative)
{
    if (me().derivatives_.count() == 0) return false;
    me().derivatives_.popFront(&derivative);
    --(*derivative).me().countDown_;
    return true;
}

int Job::countDown() const
{
    return me().countDown_;
}

bool Job::run()
{
    return me().run();
}

bool Job::finish()
{
    return me().finish();
}

const Job::State &Job::me() const
{
    return Object::me.as<State>();
}

Job::State &Job::me()
{
    return Object::me.as<State>();
}

} // namespaec cc::build
