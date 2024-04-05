/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CANgenie/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/MessageProcessor>
#include <cc/j1939/tp/protocol>
#include <cc/j1939/etp/protocol>
#include <cc/Thread>

namespace cc::j1939 {

struct MessageProcessor::State final: public Object::State
{
    explicit State(const CanMedia &media):
        media_{media}
    {}

    ~State()
    {
        wait();
    }

    void src(uint8_t address)
    {
        if (!started()) {
            src_.deplete();
            src_.insert(address);
        }
    }

    void src(const Set<uint8_t> &addresses)
    {
        if (!started()) {
            src_ = addresses;
        }
    }

    void timeout(int duration)
    {
        if (!started()) {
            timeout_ = duration;
        }
    }

    void accept(Function<bool(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t messageSize)> &&f)
    {
        if (!started()) {
            accept_ = move(f);
        }
    }

    void process(Function<void(uint32_t pgn, uint8_t src, uint8_t dst, const String &message)> &&f)
    {
        if (!started()) {
            process_ = move(f);
        }
    }

    void error(Function<void(uint32_t pgn, uint8_t src, uint8_t dst, StdException &exception)> &&f)
    {
        if (!started()) {
            error_ = move(f);
        }
    }

    void start()
    {
        if (!started()) {
            thread_ = Thread{[this]{ run(); }};
            thread_.start();
        }
    }

    bool started() const
    {
        return !thread_.isNull();
    }

    void wait()
    {
        if (started()) {
            thread_.wait();
            thread_ = Thread{};
        }
    }

    void run()
    {
        try {
            for (CanFrame frame: media_)
            {
                if (!j1939::Message{frame}) continue;
                if (!src_.contains(CanId{frame}.dst())) continue;

                j1939::CanId id{frame};

                if (tp::BroadcastAnnounce{frame})
                {
                    tp::BroadcastAnnounce bam{frame};
                    try {
                        if (accept_(id.pgn(), id.src(), id.dst(), bam.totalSize()))
                        {
                            if (process_) {
                                String message;
                                tp::receiveBroadcast(media_, bam, &message, timeout_);
                                process_(id.pgn(), id.src(), id.dst(), message);
                            }
                            else {
                                tp::receiveBroadcast(media_, bam, None{}, timeout_);
                            }
                        }
                    }
                    catch (StdException &exception)
                    {
                        error_(id.pgn(), id.src(), id.dst(), exception);
                    }
                }
                else if (tp::RequestToSend{frame})
                {
                    tp::RequestToSend rts{frame};
                    try {
                        if (accept_(id.pgn(), id.src(), id.dst(), rts.totalSize()))
                        {
                            if (process_) {
                                String message;
                                tp::receive(media_, rts, &message, timeout_);
                                process_(id.pgn(), id.src(), id.dst(), message);
                            }
                            else {
                                tp::receive(media_, rts, None{}, timeout_);
                            }
                        }
                    }
                    catch (StdException &exception)
                    {
                        error_(id.pgn(), id.src(), id.dst(), exception);
                    }
                }
                else if (etp::RequestToSend{frame})
                {
                    etp::RequestToSend rts{frame};
                    try {
                        if (accept_(id.pgn(), id.src(), id.dst(), rts.totalSize()))
                        {
                            if (process_) {
                                String message;
                                etp::receive(media_, rts, &message, timeout_);
                                process_(id.pgn(), id.src(), id.dst(), message);
                            }
                            else {
                                etp::receive(media_, rts, None{}, timeout_);
                            }
                        }
                    }
                    catch (StdException &exception)
                    {
                        error_(id.pgn(), id.src(), id.dst(), exception);
                    }
                }
                else
                {
                    try {
                        if (accept_(id.pgn(), id.src(), id.dst(), frame.size()))
                        {
                            process_(id.pgn(), id.src(), id.dst(), frame.data());
                        }
                    }
                    catch (StdException &exception)
                    {
                        error_(id.pgn(), id.src(), id.dst(), exception);
                    }
                }
            }
        }
        catch (StdException &exception)
        {
            error_(0, 0, 0, exception);
        }
    }

    CanMedia media_;
    Set<uint8_t> src_;
    int timeout_ { 750 };
    Function<bool(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t messageSize)> accept_;
    Function<void(uint32_t pgn, uint8_t src, uint8_t dst, const String &message)> process_;
    Function<void(uint32_t pgn, uint8_t src, uint8_t dst, StdException &exception)> error_;
    Thread thread_;
};

MessageProcessor::MessageProcessor()
{}

MessageProcessor::MessageProcessor(const CanMedia &media):
    Object{new State{media}}
{}

MessageProcessor &MessageProcessor::src(uint8_t address)
{
    assert(!started());
    me().src(address);
    return *this;
}

MessageProcessor &MessageProcessor::src(const Set<uint8_t> &addresses)
{
    assert(!started());
    me().src(addresses);
    return *this;
}

MessageProcessor &MessageProcessor::timeout(int duration)
{
    assert(!started());
    me().timeout(duration);
    return *this;
}

MessageProcessor &MessageProcessor::accept(Function<bool(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t messageSize)> &&f)
{
    assert(!started());
    me().accept(move(f));
    return *this;
}

MessageProcessor &MessageProcessor::process(Function<void(uint32_t pgn, uint8_t src, uint8_t dst, const String &message)> &&f)
{
    assert(!started());
    me().process(move(f));
    return *this;
}

MessageProcessor &MessageProcessor::error(Function<void(uint32_t pgn, uint8_t src, uint8_t dst, StdException &exception)> &&f)
{
    assert(!started());
    me().error(move(f));
    return *this;
}

MessageProcessor &MessageProcessor::start()
{
    assert(!started());
    me().start();
    return *this;
}

bool MessageProcessor::started() const
{
    return me().started();
}

void MessageProcessor::wait()
{
    assert(started());
    me().wait();
}

MessageProcessor::State &MessageProcessor::me()
{
    return Object::me.as<State>();
}

const MessageProcessor::State &MessageProcessor::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::j1939
