/*
 * Copyright (C) 2021-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/JobState>

namespace cc::build {

Job::Job(const String &command):
    Object{new State{command}}
{}

Job::Job(const List<String> &args):
    Object{new State{args}}
{}

Job::Job(State *newState):
    Object{newState}
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

bool Job::reported() const
{
    return me().reported_;
}

void Job::setReported(bool on)
{
    me().reported_ = on;
}

bool Job::run()
{
    return me().run();
}

void Job::wait()
{
    me().wait();
}

void Job::notify()
{
    me().fin_.release();
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
