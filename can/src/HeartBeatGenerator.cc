/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/HeartBeatGenerator>
#include <cc/can/HeartBeat>
#include <cc/Worker>
#include <cc/Channel>
#include <cc/System>

namespace cc {
namespace can {

Ref<HeartBeatGenerator> HeartBeatGenerator::create(int nodeId, CanMedia *media)
{
    Ref<HeartBeatGenerator> self = new HeartBeatGenerator{nodeId, media};
    self->bootstrap();
    return self;
}

HeartBeatGenerator::HeartBeatGenerator(int nodeId, CanMedia *media):
    nodeId_{nodeId},
    media_{media},
    commandChannel_{CommandChannel::create()},
    beat_{Beat::create()}
{}

HeartBeatGenerator::~HeartBeatGenerator()
{
    commandChannel_->pushBack(nullptr);
}

void HeartBeatGenerator::bootstrap()
{
    worker_ = Worker::start([this]{ run(); });
}

uint16_t HeartBeatGenerator::interval() const
{
    return requestedInterval_;
}

void HeartBeatGenerator::setInterval(uint16_t newInterval)
{
    if (requestedInterval_ != newInterval) {
        requestedInterval_ = newInterval;
        commandChannel_->pushBack(SetInterval::create(newInterval));
    }
}

NodeState HeartBeatGenerator::nodeState() const
{
    return requestedNodeState_;
}

void HeartBeatGenerator::setNodeState(NodeState newNodeState)
{
    if (requestedNodeState_ != newNodeState) {
        requestedNodeState_ = newNodeState;
        commandChannel_->pushBack(SetNodeState::create(newNodeState));
    }
}

bool HeartBeatGenerator::getNextCommand(Ref<Command> *command)
{
    if (interval_ > 0) {
        if (!commandChannel_->popFrontBefore(System::now() + interval_, command))
            *command = beat_;
    }
    else
        commandChannel_->popFront(command);

    return *command;
}

void HeartBeatGenerator::beat()
{
    if (
        interval_ > 0 && (
            nodeState_ == NodeState::Operational ||
            nodeState_ == NodeState::PreOperational
        )
    ) {
        media_->writeFrame(
            HeartBeat::createFrame(nodeId_, nodeState_)
        );
    }
}

void HeartBeatGenerator::establishNewNodeState(NodeState newNodeState)
{
    if (nodeState_ != newNodeState)
    {
        nodeState_ = newNodeState;

        if (newNodeState == NodeState::BootUp) {
            media_->writeFrame(
                HeartBeat::createFrame(nodeId_, NodeState::BootUp)
            );
        }
        else
            beat();
    }
}

void HeartBeatGenerator::run()
{
    for (Ref<Command> command; getNextCommand(&command);) {
        switch (command->type()) {
            case Command::Type::Beat: {
                beat();
                break;
            }
            case Command::Type::SetInterval: {
                interval_ = double(Object::cast<const SetInterval *>(command)->newInterval()) / 1000;
                break;
            }
            case Command::Type::SetNodeState: {
                establishNewNodeState(Object::cast<const SetNodeState *>(command)->newNodeState());
                break;
            }
        }
    }
}

}} // namespace cc::can
