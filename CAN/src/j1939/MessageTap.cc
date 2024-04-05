/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/j1939/MessageTap>
#include <cc/j1939/tp/BroadcastAnnounce>
#include <cc/j1939/tp/RequestToSend>
#include <cc/j1939/tp/DataPacket>
#include <cc/j1939/tp/EndOfMessage>
#include <cc/j1939/tp/Abort>
#include <cc/j1939/etp/RequestToSend>
#include <cc/j1939/etp/DataPacket>
#include <cc/j1939/etp/DataPacketOffset>
#include <cc/j1939/etp/EndOfMessage>
#include <cc/j1939/etp/Abort>
#include <cc/j1939/Connection>
#include <cc/Thread>
#include <cc/Set>

namespace cc::j1939 {

struct MessageTap::State final: public Object::State
{
    State(const CanMedia &media):
        receive_{
            [source=media](Out<CanFrame> frame) mutable {
                return source.read(&frame);
            }
        }
    {}

    State(Function<bool(Out<CanFrame>)> &&f):
        receive_{move(f)}
    {}

    void accept(Function<bool(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t size)> &&f)
    {
        if (!started()) {
            accept_ = move(f);
        }
    }

    void process(Function<void(const Datagram &)> &&f)
    {
        if (!started()) {
            process_ = move(f);
        }
    }

    void error(Function<void(StdException &)> &&f)
    {
        if (!started()) {
            error_ = move(f);
        }
    }

    void start()
    {
        assert(receive_ && process_);

        if (receive_ && process_) {
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
        }
    }

    void run()
    {
        try {
            for (CanFrame frame; receive_(&frame);)
            {
                if (!j1939::Message{frame})
                    ;
                else if (readTp(frame))
                    ;
                else if (readEtp(frame))
                    ;
                else {
                    j1939::CanId id{frame};
                    if (!accept_ || accept_(id.pgn(), id.src(), id.dst(), frame.size())) {
                        process_(Datagram{id.pgn(), id.src(), id.dst(), frame.data()});
                    }
                }
            }
        }
        catch (StdException &ex)
        {
            error_(ex);
        }
        #ifdef NDEBUG
        catch (...)
        {}
        #endif
    }

    bool readTp(const CanFrame &frame)
    {
        bool tp = false;

        if (tp::ConnectionManagement{frame}) {
            if (tp::BroadcastAnnounce{frame}) {
                tp::BroadcastAnnounce bam{frame};
                if (!accept_ || accept_(bam.payloadPgn(), bam.src(), bam.dst(), bam.totalSize())) {
                    connections_.establish({bam.payloadPgn(), bam.src(), bam.dst(), bam.totalSize()});
                }
            }
            else if (tp::RequestToSend{frame}) {
                tp::RequestToSend rts{frame};
                if (!accept_ || accept_(rts.payloadPgn(), rts.src(), rts.dst(), rts.totalSize())) {
                    connections_.establish({rts.payloadPgn(), rts.src(), rts.dst(), rts.totalSize()});
                }
            }
            else if (tp::EndOfMessage{frame}) {
                tp::EndOfMessage ack{frame};
                Connection connection;
                if (connections_.lookup(Connection::id(ack.dst(), ack.src()), &connection)) {
                    String message;
                    if (connection.collect(&message)) {
                        process_(Datagram{connection.pgn(), connection.src(), connection.dst(), message});
                    }
                }
            }
            else if (tp::Abort{frame}) {
                tp::Abort abort{frame};
                connections_.remove(Connection::id(abort.dst(), abort.src()));
                connections_.remove(Connection::id(abort.src(), abort.dst()));
            }

            tp = true;
        }
        else if (tp::DataPacket{frame}) {
            tp::DataPacket packet{frame};
            Connection connection;
            if (connections_.lookup(Connection::id(packet.src(), packet.dst()), &connection)) {
                if (connection.deposit(packet.packetNumber(), frame) && packet.dst() == 0xFF) {
                    if (packet.packetNumber() == connection.packetCount()) {
                        String message;
                        if (connection.collect(&message)) {
                            process_(Datagram{connection.pgn(), connection.src(), connection.dst(), message});
                        }
                    }
                }
            }

            tp = true;
        }

        return tp;
    }

    bool readEtp(const CanFrame &frame)
    {
        bool etp = false;

        if (etp::ConnectionManagement{frame}) {
            if (etp::RequestToSend{frame}) {
                etp::RequestToSend rts{frame};
                if (!accept_ || accept_(rts.payloadPgn(), rts.src(), rts.dst(), rts.totalSize())) {
                    connections_.establish({rts.payloadPgn(), rts.src(), rts.dst(), rts.totalSize()});
                }
            }
            else if (etp::DataPacketOffset{frame}) {
                etp::DataPacketOffset dpo{frame};
                Connection connection;
                if (connections_.lookup(Connection::id(dpo.src(), dpo.dst()), &connection)) {
                    connection.setPacketOffset(dpo.packetOffset());
                }
            }
            else if (etp::EndOfMessage{frame}) {
                etp::EndOfMessage ack{frame};
                Connection connection;
                if (connections_.lookup(Connection::id(ack.dst(), ack.src()), &connection)) {
                    String message;
                    if (connection.collect(&message)) {
                        process_(Datagram{connection.pgn(), connection.src(), connection.dst(), message});
                    }
                }
            }
            else if (tp::Abort{frame}) {
                etp::Abort abort{frame};
                connections_.remove(Connection::id(abort.dst(), abort.src()));
                connections_.remove(Connection::id(abort.src(), abort.dst()));
            }

            etp = true;
        }
        else if (etp::DataPacket{frame}) {
            etp::DataPacket packet{frame};
            Connection connection;
            if (connections_.lookup(Connection::id(packet.src(), packet.dst()), &connection)) {
                connection.deposit(packet.packetNumber(), frame);
            }

            etp = true;
        }

        return etp;
    }

    Function<bool(Out<CanFrame>)> receive_;
    Function<bool(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t size)> accept_;
    Function<void(const Datagram &)> process_;
    Function<void(StdException &)> error_;
    Set<Connection> connections_;
    Thread thread_;
};

MessageTap::MessageTap()
{}

MessageTap::MessageTap(const CanMedia &media):
    Object{new State{media}}
{}

MessageTap::MessageTap(Function<bool(Out<CanFrame>)> &&f):
    Object{new State{move(f)}}
{}

MessageTap &MessageTap::accept(Function<bool(uint32_t pgn, uint8_t src, uint8_t dst, uint32_t size)> &&f)
{
    assert(!started());
    me().accept(move(f));
    return *this;
}

MessageTap &MessageTap::process(Function<void(const Datagram &)> &&f)
{
    assert(!started());
    me().process(move(f));
    return *this;
}

MessageTap &MessageTap::error(Function<void(StdException &)> &&f)
{
    assert(!started());
    me().error(move(f));
    return *this;
}

MessageTap &MessageTap::start()
{
    assert(!started());
    me().start();
    return *this;
}

bool MessageTap::started() const
{
    return me().started();
}

void MessageTap::wait()
{
    assert(started());
    me().wait();
}

MessageTap::State &MessageTap::me()
{
    return Object::me.as<State>();
}

const MessageTap::State &MessageTap::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::j1939
