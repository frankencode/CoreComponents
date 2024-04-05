/*
 * Copyright (C) 2019-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/can_open/Node>
#include <cc/can_open/sdo/WriteRequest>
#include <cc/can_open/sdo/WriteReply>
#include <cc/can_open/sdo/WriteSegmentRequest>
#include <cc/can_open/sdo/WriteSegmentReply>
#include <cc/can_open/sdo/ReadRequest>
#include <cc/can_open/sdo/ReadReply>
#include <cc/can_open/sdo/ReadSegmentRequest>
#include <cc/can_open/sdo/ReadSegmentReply>
#include <cc/can_open/sdo/BlockWriteInitRequest>
#include <cc/can_open/sdo/BlockWriteInitReply>
#include <cc/can_open/sdo/BlockWriteSegmentRequest>
#include <cc/can_open/sdo/BlockWriteAckReply>
#include <cc/can_open/sdo/BlockWriteEndRequest>
#include <cc/can_open/sdo/BlockWriteEndReply>
#include <cc/can_open/sdo/BlockReadInitRequest>
#include <cc/can_open/sdo/BlockReadInitReply>
#include <cc/can_open/sdo/BlockReadStartRequest>
#include <cc/can_open/sdo/BlockReadSegmentReply>
#include <cc/can_open/sdo/BlockReadAckRequest>
#include <cc/can_open/sdo/BlockReadEndReply>
#include <cc/can_open/sdo/BlockReadEndRequest>
#include <cc/can_open/sdo/AbortRequest>
#include <cc/can_open/sdo/AbortReply>
#include <cc/can_open/sdo/protocol>
#include <cc/can_open/Crc16Sink>
#include <cc/can_open/Payload>
#include <cc/stdio>
//#include <cc/can/EventMonitor> // TODO...
//#include <cc/can/CanNodeFeed> // TODO...

namespace cc::can_open {

Node::State::State(const CanMedia &media, int nodeId, int timeout):
    media_{media},
    nodeId_{nodeId},
    timeout_{timeout},
    heartBeatGenerator_{nodeId, media}
{}

Node::State::~State()
{
    shutdown();
}

void Node::State::initSettings()
{}

NodeState Node::State::nodeState() const
{
    return heartBeatGenerator_.nodeState();
}

void Node::State::setNodeState(NodeState newNodeState)
{
    heartBeatGenerator_.setNodeState(newNodeState);
}

bool Node::State::autoStart() const
{
    return false;
}

void Node::State::nodeStart()
{}

void Node::State::nodeStop()
{}

void Node::State::nodeHalt()
{}

void Node::State::nodeReboot()
{}

void Node::State::nodeReset()
{}

Bytes Node::State::readObject(Selector selector) const
{
    Bytes data;

    switch (+selector)
    {
        case Selector::ErrorRegister: {
            data = Payload<uint8_t>::make(+errorRegister_);
            break;
        }
        case Selector::HeartBeatTime: {
            data = Payload<uint16_t>::make(heartBeatGenerator_.interval());
            break;
        }
        default: {
            throw sdo::Aborted{sdo::ErrorCode::SelectorInvalid};
        }
    };

    return data;
}

void Node::State::writeObject(Selector selector, const Bytes &data)
{
    switch (+selector)
    {
        case Selector::ErrorRegister: {
            errorRegister_ =
                static_cast<ErrorRegister>(
                    Payload<uint8_t>::read(data, +ErrorRegister::None)
                );
            break;
        }
        case Selector::HeartBeatTime: {
            heartBeatGenerator_.setInterval(
                Payload<uint16_t>::read(data, heartBeatGenerator_.interval())
            );
            break;
        }
        default: {
            throw sdo::Aborted{sdo::ErrorCode::SelectorInvalid};
        }
    };
}

bool Node::State::expeditedTransferIsEnabled() const
{
    return true;
}

ErrorRegister Node::State::errorRegister() const
{
    return errorRegister_;
}

void Node::State::setErrorRegister(ErrorRegister flags)
{
    errorRegister_ = flags;
}

void Node::State::issueEmergency(Emergency::Type emergencyType, uint64_t deviceError)
{
    media_.write(Emergency::createFrame(nodeId_, emergencyType, errorRegister(), deviceError));
}

void Node::State::start()
{
    setNodeState(NodeState::BootUp);

    initSettings();
    // initEventHandlers();

    setNodeState(NodeState::PreOperational);

    if (autoStart()) {
        setNodeState(NodeState::Operational);
    }

    thread_ = Thread{[this]{ run(); }};
    thread_.start();
}

void Node::State::shutdown()
{
    if (thread_) {
        thread_.wait();
        thread_ = Thread{};
    }
}

void Node::State::run()
{
    while (true) {
        CanFrame frame;
        if (!media_.read(&frame)) break;

        try {
            NetworkCommand command{frame};
            if (command) {
                if (command.targetId() == nodeId_ || command.targetId() == 0) {
                    handleNodeCommand(command.specifier());
                    continue;
                }
            }
        }
        catch (Exception &ex) {
            ferr() << "NMT failed: " << ex.message() << nl;
        }

        try {
            sdo::Request request{frame};
            if (request) {
                if (request.serverId() != nodeId_) continue;

                #if 0 // TODO
                if (!(
                    nodeState() == NodeState::PreOperational ||
                    nodeState() == NodeState::Operational
                ))
                    continue;
                #endif

                switch (request.specifier()) {
                    case sdo::Request::Specifier::Write: {
                        handleWriteRequest(frame);
                        break;
                    }
                    case sdo::Request::Specifier::WriteSegment: {
                        handleWriteSegmentRequest(frame);
                        break;
                    }
                    case sdo::Request::Specifier::Read: {
                        handleReadRequest(frame);
                        break;
                    }
                    case sdo::Request::Specifier::ReadSegment: {
                        handleReadSegmentRequest(frame);
                        break;
                    }
                    case sdo::Request::Specifier::BlockWrite: {
                        handleBlockWriteRequest(frame);
                        break;
                    }
                    case sdo::Request::Specifier::BlockRead: {
                        handleBlockReadRequest(frame);
                        break;
                    }
                    default: ;
                };

                continue;
            }
        }
        catch (sdo::Error &error) {
            ferr() << "SDO failed: " << error.message() << nl;
        }

        /*if (eventFeed_) {
            if (nodeState() != NodeState::Operational) continue;
            eventFeed_->feedFrame(frame);
        }*/ // TODO...
    }
}

void Node::State::handleNodeCommand(NetworkCommand::Specifier commandSpecifier)
{
    switch (commandSpecifier)
    {
        case NetworkCommand::Specifier::Start: {
            if (
                nodeState() == NodeState::PreOperational ||
                nodeState() == NodeState::Stopped
            ) {
                nodeStart();
                setNodeState(NodeState::Operational);
            }
            break;
        }
        case NetworkCommand::Specifier::Stop: {
            if (
                nodeState() == NodeState::PreOperational ||
                nodeState() == NodeState::Operational
            ) {
                nodeStop();
                setNodeState(NodeState::Stopped);
            }
            break;
        }
        case NetworkCommand::Specifier::Halt: {
            if (
                nodeState() == NodeState::Operational ||
                nodeState() == NodeState::Stopped
            ) {
                nodeHalt();
                setNodeState(NodeState::PreOperational);
            }
            break;
        }
        case NetworkCommand::Specifier::Reboot: {
            if (
                nodeState() != NodeState::BootUp
            ) {
                setNodeState(NodeState::BootUp);
                nodeReboot();
                setNodeState(NodeState::PreOperational);
            }
            break;
        }
        case NetworkCommand::Specifier::Reset: {
            if (
                nodeState() != NodeState::BootUp
            ) {
                setNodeState(NodeState::BootUp);
                nodeReset();
                setNodeState(NodeState::PreOperational);
            }
            break;
        }
    };
}

void Node::State::handleWriteRequest(const CanFrame &head)
{
    if (!sdo::WriteRequest{head}) return;

    Selector selector = sdo::WriteRequest{head}.selector();

    try {
        if (sdo::WriteRequest{head}.hasExpeditedData())
        {
            writeObject(selector, sdo::WriteRequest{head}.expeditedData());

            writeHead_ = CanFrame{};
        }
        else {
            if (sdo::WriteRequest{head}.totalSize() > static_cast<uint32_t>(buffer_.count())) {
                throw sdo::ErrorCode::OutOfMemory;
            }

            writeHead_ = head;
            fill_ = 0;
        }

        readHead_ = CanFrame{};

        media_.write(
            sdo::WriteReply::createFrame(nodeId_, selector)
        );
    }
    catch (sdo::ErrorCode &error) {
        canAbort(selector, error);
    }
}

void Node::State::handleWriteSegmentRequest(const CanFrame &tail)
{
    if (writeHead_.canId() == 0) return;

    try {
        if (!sdo::WriteSegmentRequest{tail}) {
            throw sdo::ErrorCode::GeneralError;
        }

        CanFrame replyFrame = sdo::WriteSegmentReply::createFrame(nodeId_, fill_);

        if (sdo::WriteSegmentRequest{tail}.toggle() != sdo::WriteSegmentReply{replyFrame}.toggle()) {
            throw sdo::ErrorCode::ToggleBitInvalid;
        }

        {
            const int segmentDataCount = sdo::WriteSegmentRequest{tail}.dataCount();
            if (fill_ + segmentDataCount > buffer_.count()) {
                throw sdo::ErrorCode::OutOfMemory;
            }

            for (int i = 0; i < segmentDataCount; ++i) {
                buffer_.at(fill_) = sdo::WriteSegmentRequest{tail}.dataAt(i);
                ++fill_;
            }
        }

        if (sdo::WriteSegmentRequest{tail}.isLast()) {
            Bytes data = buffer_.copy(0, fill_);
            writeObject(sdo::WriteRequest{writeHead_}.selector(), data);
            writeHead_ = CanFrame{};
        }

        media_.write(replyFrame);
    }
    catch (sdo::ErrorCode &error) {
        canAbort(sdo::WriteRequest{writeHead_}.selector(), error);
    }
}

void Node::State::handleReadRequest(const CanFrame &head)
{
    if (!sdo::ReadRequest{head}) return;

    Selector selector = sdo::ReadRequest{head}.selector();

    try {
        Bytes data = readObject(selector);

        CanFrame frame = sdo::ReadReply::createFrame(nodeId_, selector, data);

        if (sdo::ReadReply{frame}.hasExpeditedData())
        {
            readHead_ = CanFrame{};
        }
        else {
            if (data.count() > buffer_.count()) {
                throw sdo::ErrorCode::OutOfMemory;
            }

            readHead_ = head;
            data.copyTo(&buffer_);
            fill_ = data.count();
            offset_ = 0;
        }

        writeHead_ = CanFrame{};

        media_.write(frame);
    }
    catch (sdo::ErrorCode &error) {
        canAbort(selector, error);
    }
}

void Node::State::handleReadSegmentRequest(const CanFrame &tail)
{
    if (readHead_.canId() == 0) return;

    try {
        if (!sdo::ReadSegmentRequest{tail}) {
            throw sdo::ErrorCode::GeneralError;
        }

        CanFrame frame = sdo::ReadSegmentReply::createFrame(nodeId_, buffer_, fill_, offset_);

        if (sdo::ReadSegmentRequest{tail}.toggle() != sdo::ReadSegmentReply{frame}.toggle()) {
            throw sdo::ErrorCode::ToggleBitInvalid;
        }

        offset_ += sdo::ReadSegmentReply{frame}.dataCount();

        media_.write(frame);
        if (sdo::ReadSegmentReply{frame}.isLast()) readHead_ = CanFrame{};
    }
    catch (sdo::ErrorCode &error) {
        canAbort(sdo::ReadRequest{readHead_}.selector(), error);
    }
}

void Node::State::handleBlockWriteRequest(const CanFrame &head)
{
    if (!sdo::BlockWriteInitRequest{head}) return;

    Selector selector = sdo::BlockWriteInitRequest{head}.selector();

    try {
        uint32_t totalLeft = sdo::BlockWriteInitRequest{head}.totalSize();
        bool crcSupport = sdo::BlockWriteInitRequest{head}.crcSupport();

        if (totalLeft > uint32_t(buffer_.count())) {
            throw sdo::ErrorCode::OutOfMemory;
        }

        if (totalLeft == 0) totalLeft = buffer_.count();

        int blockSize = 0x7F;
        if (0 < totalLeft && totalLeft < 7 * 0x7F) {
            blockSize = totalLeft / 7 + (totalLeft % 7 > 0);
        }

        media_.write(
            sdo::BlockWriteInitReply::createFrame(nodeId_, selector, blockSize)
        );

        fill_ = 0;

        for (bool go = true; go;)
        {
            int segmentsCount = 0;

            while (segmentsCount < blockSize && go)
            {
                auto frame = getNextRequest();

                if (!sdo::BlockWriteSegmentRequest{frame}) {
                    throw sdo::ErrorCode::GeneralError;
                }

                if (sdo::BlockWriteSegmentRequest{frame}.sequenceNumber() > segmentsCount + 1) break;

                sdo::BlockWriteSegmentRequest{frame}.segmentData().copyToRange(&buffer_, fill_ + 7 * segmentsCount, fill_ + 7 * (segmentsCount + 1));
                ++segmentsCount;

                if (sdo::BlockWriteSegmentRequest{frame}.isLast()) go = false;
            }

            if (segmentsCount == blockSize || !go) {
                totalLeft -= 7 * segmentsCount;
                fill_ += 7 * segmentsCount;
            }

            if (0 < totalLeft && totalLeft < 7 * 0x7F) {
                blockSize = totalLeft / 7 + (totalLeft % 7 > 0);
            }

            if (totalLeft <= 0 && go) throw sdo::ErrorCode::GeneralError;

            media_.write(
                sdo::BlockWriteAckReply::createFrame(nodeId_, segmentsCount, blockSize)
            );
        }

        auto frame = getNextRequest();

        if (sdo::AbortRequest{frame}) {
            throw sdo::AbortRequest{frame}.error();
        }

        if (!sdo::BlockWriteEndRequest{frame}) {
            throw sdo::ErrorCode::GeneralError;
        }

        fill_ -= 7 - sdo::BlockWriteEndRequest{frame}.lastSegmentDataCount();
        Bytes data = buffer_.copy(0, fill_);
        if (crcSupport) {
            uint16_t checkSum = crc16(data);
            if (checkSum != sdo::BlockWriteEndRequest{frame}.crc()) {
                throw sdo::ErrorCode::CrcError;
            }
        }

        writeObject(selector, data);

        media_.write(
            sdo::BlockWriteEndReply::createFrame(nodeId_)
        );
    }
    catch (sdo::ErrorCode &error) {
        canAbort(selector, error);
    }
}

void Node::State::handleBlockReadRequest(const CanFrame &head)
{
    if (!sdo::BlockReadInitRequest{head}) return;

    Selector selector = sdo::BlockReadInitRequest{head}.selector();

    try {
        Bytes data = readObject(selector);

        if (data.count() < sdo::BlockReadInitRequest{head}.switchThreshold()) {
            handleReadRequest(
                sdo::ReadRequest::createFrame(
                    sdo::BlockReadInitRequest{head}.serverId(),
                    sdo::BlockReadInitRequest{head}.selector()
                )
            );

            return;
        }

        media_.write(
            sdo::BlockReadInitReply::createFrame(nodeId_, selector, data)
        );

        {
            auto frame = getNextRequest();
            if (!sdo::BlockReadStartRequest{frame}) {
                throw sdo::ErrorCode::GeneralError;
            }
        }

        int blockSize = sdo::BlockReadInitRequest{head}.blockSize();
        int offset = 0;
        int seqNum = 1;

        for (bool go = true; go;)
        {
            for (;seqNum <= blockSize && go; ++seqNum) {
                auto frame = sdo::BlockReadSegmentReply::createFrame(nodeId_, data, offset + 7 * (seqNum - 1), seqNum);
                media_.write(frame);
                go = !sdo::BlockReadSegmentReply{frame}.isLast();
            }

            auto frame = getNextRequest();
            if (!sdo::BlockReadAckRequest{frame})
                throw sdo::ErrorCode::GeneralError;

            if (sdo::BlockReadAckRequest{frame}.sequenceNumber() == blockSize || !go) {
                offset += 7 * (seqNum - 1);
                seqNum = 1;
            }
            else {
                seqNum = sdo::BlockReadAckRequest{frame}.sequenceNumber() + 1;
            }

            blockSize = sdo::BlockReadAckRequest{frame}.newBlockSize();
        }

        media_.write(
            sdo::BlockReadEndReply::createFrame(nodeId_, data, sdo::BlockReadInitRequest{head}.crcSupport())
        );

        auto frame = getNextRequest();

        if (sdo::AbortRequest{frame}) {
            throw sdo::AbortRequest{frame}.error();
        }

        if (!sdo::BlockReadEndRequest{frame}) {
            throw sdo::ErrorCode::GeneralError;
        }
    }
    catch (sdo::ErrorCode &error) {
        canAbort(selector, error);
    }
}

CanFrame Node::State::getNextRequest()
{
    CanFrame frame;

    while (true) {
        if (!media_.wait(timeout_)) {
            throw sdo::ErrorCode::ServiceDataTimeout;
        }

        if (!media_.read(&frame)) {
            throw sdo::InputExhaustion{};
        }

        if (!sdo::Request{frame}) continue;
        if (sdo::Request{frame}.serverId() != nodeId_) continue;

        break;
    }

    return frame;
}

void Node::State::canAbort(Selector selector, sdo::ErrorCode error)
{
    media_.write(
        sdo::AbortReply::createFrame(nodeId_, selector, error)
    );

    writeHead_ = CanFrame{};
    readHead_ = CanFrame{};
}

Node::Node(State *newState):
    Object{newState}
{}

} // namespace cc::can_open
