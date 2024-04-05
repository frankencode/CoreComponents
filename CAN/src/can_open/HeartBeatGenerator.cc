/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/HeartBeatGenerator>
#include <cc/can_open/HeartBeat>
#include <cc/System>
#include <cc/Thread>
#include <cc/Channel>

namespace cc::can_open {

struct HeartBeatGenerator::State final: public Object::State
{
    class Command: public Object
    {
    public:
        enum class Type {
            SetInterval,
            SetNodeState,
            Beat
        };

        Command() = default;

        Type type() const { return me().type_; }

    protected:
        struct State: public Object::State
        {
            explicit State(Type type):
                type_{type}
            {}

            Type type_;
        };

        explicit Command(State *newState):
            Object{newState}
        {}

        const State &me() const { return Object::me.as<State>(); }
    };

    class SetInterval final: public Command
    {
    public:
        explicit SetInterval(uint16_t newInterval):
            Command{new State{newInterval}}
        {}

        uint16_t newInterval() const { return me().newInterval_; }

    private:
        friend class Object;

        struct State final: public Command::State
        {
            explicit State(uint16_t newInterval):
                Command::State{Type::SetInterval},
                newInterval_{newInterval}
            {}

            uint16_t newInterval_;
        };

        SetInterval() = default;

        const State &me() const { return Object::me.as<State>(); }
    };

    class SetNodeState final: public Command
    {
    public:
        explicit SetNodeState(NodeState newNodeState):
            Command{new State{newNodeState}}
        {}

        NodeState newNodeState() const { return me().newNodeState_; }

    private:
        friend class Object;

        struct State final: public Command::State
        {
            explicit State(NodeState newNodeState):
                Command::State{Type::SetNodeState},
                newNodeState_{newNodeState}
            {}

            NodeState newNodeState_;
        };

        SetNodeState() = default;

        const State &me() const { return Object::me.as<State>(); }
    };

    class Beat final: public Command
    {
    public:
        Beat():
            Command{new State}
        {}

    private:
        struct State final: public Command::State
        {
            State():
                Command::State{Type::Beat}
            {}
        };
    };

    State(int nodeId, const CanMedia &media):
        nodeId_{nodeId},
        media_{media}
    {}

    ~State()
    {
        if (thread_) {
            commandChannel_.pushBack(Command{});
            thread_.wait();
            thread_ = Thread{};
        }
    }

    uint16_t interval() const
    {
        return requestedInterval_;
    }

    void setInterval(uint16_t newInterval)
    {
        if (requestedInterval_ != newInterval) {
            requestedInterval_ = newInterval;
            commandChannel_.pushBack(SetInterval{newInterval});
        }
    }

    NodeState nodeState() const
    {
        return requestedNodeState_;
    }

    void setNodeState(NodeState newNodeState)
    {
        if (requestedNodeState_ != newNodeState) {
            requestedNodeState_ = newNodeState;
            commandChannel_.pushBack(SetNodeState{newNodeState});
        }
    }

    void start()
    {
        thread_ = Thread{[this]{ run(); }};
        thread_.start();
    }

    bool getNextCommand(Out<Command> command)
    {
        if (interval_ > 0) {
            if (!commandChannel_.popFrontBefore(System::now() + interval_, &command)) {
                command = beat_;
            }
        }
        else {
            commandChannel_.popFront(&command);
        }

        return !command->isNull();
    }

    void beat()
    {
        if (
            interval_ > 0 && (
                nodeState_ == NodeState::Operational ||
                nodeState_ == NodeState::PreOperational
            )
        ) {
            media_.write(
                HeartBeat::createFrame(nodeId_, nodeState_)
            );
        }
    }

    void establishNewNodeState(NodeState newNodeState)
    {
        if (nodeState_ != newNodeState)
        {
            nodeState_ = newNodeState;

            if (newNodeState == NodeState::BootUp) {
                media_.write(
                    HeartBeat::createFrame(nodeId_, NodeState::BootUp)
                );
            }
            else {
                beat();
            }
        }
    }

    void run()
    {
        for (Command command; getNextCommand(&command);)
        {
            switch (command.type())
            {
                case Command::Type::Beat: {
                    beat();
                    break;
                }
                case Command::Type::SetInterval: {
                    interval_ = static_cast<double>(command.as<SetInterval>().newInterval()) / 1000;
                    break;
                }
                case Command::Type::SetNodeState: {
                    establishNewNodeState(command.as<SetNodeState>().newNodeState());
                    break;
                }
            }
        }
    }

    int nodeId_;
    CanMedia media_;

    Thread thread_;
    Channel<Command> commandChannel_;
    Beat beat_;

    double interval_ { 0 };
    NodeState nodeState_ { NodeState::Undefined };

    uint16_t requestedInterval_ { 0 };
    NodeState requestedNodeState_ { NodeState::Undefined };
};

HeartBeatGenerator::HeartBeatGenerator(int nodeId, const CanMedia &media):
    Object{new State{nodeId, media}}
{}

uint16_t HeartBeatGenerator::interval() const
{
    return me().interval();
}

void HeartBeatGenerator::setInterval(uint16_t newInterval)
{
    me().setInterval(newInterval);
}

NodeState HeartBeatGenerator::nodeState() const
{
    return me().nodeState();
}

void HeartBeatGenerator::setNodeState(NodeState newNodeState)
{
    me().setNodeState(newNodeState);
}

HeartBeatGenerator::State &HeartBeatGenerator::me()
{
    return Object::me.as<State>();
}

const HeartBeatGenerator::State &HeartBeatGenerator::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::can_open
