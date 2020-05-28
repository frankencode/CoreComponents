/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanNode>
#include <cc/can/HeartBeat>
#include <cc/can/ServiceRequest>
#include <cc/can/WriteRequest>
#include <cc/can/WriteReply>
#include <cc/can/WriteSegmentRequest>
#include <cc/can/WriteSegmentReply>
#include <cc/can/ReadRequest>
#include <cc/can/ReadReply>
#include <cc/can/ReadSegmentRequest>
#include <cc/can/ReadSegmentReply>
#include <cc/can/BlockWriteInitRequest>
#include <cc/can/BlockWriteInitReply>
#include <cc/can/BlockWriteSegmentRequest>
#include <cc/can/BlockWriteAckReply>
#include <cc/can/BlockWriteEndRequest>
#include <cc/can/BlockWriteEndReply>
#include <cc/can/BlockReadInitRequest>
#include <cc/can/BlockReadInitReply>
#include <cc/can/BlockReadStartRequest>
#include <cc/can/BlockReadSegmentReply>
#include <cc/can/BlockReadAckRequest>
#include <cc/can/BlockReadEndReply>
#include <cc/can/BlockReadEndRequest>
#include <cc/can/Crc16Sink>
#include <cc/can/AbortRequest>
#include <cc/can/AbortReply>
#include <cc/can/EventMonitor>
#include <cc/can/CanNodeFeed>
#include <cc/can/HeartBeatGenerator>
#include <cc/can/BinaryValue>
#include <cc/Worker>
#include <cc/stdio>
#include <cc/debug> // DEBUG

namespace cc {
namespace can {

CanNode::CanNode(CanMedia *media, int nodeId, int timeout):
    media_{media},
    nodeId_{nodeId},
    timeout_{timeout},
    buffer_{String::allocate(65536)},
    fill_{0},
    heartBeatGenerator_{HeartBeatGenerator::create(nodeId_, media_)}
{}

CanNode::~CanNode()
{}

void CanNode::init()
{
    setNodeState(NodeState::BootUp);

    initSettings();
    initEventHandlers();

    setNodeState(NodeState::PreOperational);

    if (autoStart())
        setNodeState(NodeState::Operational);

    if (monitor_)
        monitor_->start();

    Ref<CanNode> self = this;
    worker_ = Worker::start([=]{ self->run(); });
}

void CanNode::initSettings()
{}

void CanNode::initEventHandlers()
{}

NodeState CanNode::nodeState() const
{
    return heartBeatGenerator_->nodeState();
}

void CanNode::setNodeState(NodeState newNodeState)
{
    heartBeatGenerator_->setNodeState(newNodeState);
}

EventMonitor *CanNode::monitor()
{
    if (worker_) return nullptr;

    if (!monitor_) {
        eventFeed_ = CanNodeFeed::create(media_);
        monitor_ = EventMonitor::create(eventFeed_);
    }
    return monitor_;
}

void CanNode::nodeStart()
{}

void CanNode::nodeStop()
{}

void CanNode::nodeHalt()
{}

void CanNode::nodeReboot()
{
    // TODO...
}

void CanNode::nodeReset()
{
    // TODO...
}

String CanNode::readServiceData(Selector::Key key) const
{
    String data;

    switch (key) {
        case DictionaryKey::ErrorRegister: {
            data =
                BinaryValue<uint8_t>::make(
                    +errorRegister_
                );
        }
        case DictionaryKey::ProducerHeartBeatTime: {
            data =
                BinaryValue<uint16_t>::make(
                    heartBeatGenerator_->interval()
                );
        }
        default: {
            throw CanAbort{Abort::Reason::SelectorInvalid};
        }
    };

    return data;
}

void CanNode::writeServiceData(Selector::Key key, const String &data)
{
    switch (key) {
        case DictionaryKey::ErrorRegister: {
            errorRegister_ =
                static_cast<ErrorRegister>(
                    BinaryValue<uint8_t>::read(
                        data,
                        +ErrorRegister::None
                    )
                );
        }
        case DictionaryKey::ProducerHeartBeatTime: {
            heartBeatGenerator_->setInterval(
                BinaryValue<uint16_t>::read(
                    data,
                    heartBeatGenerator_->interval()
                )
            );
        }
        default: {
            throw CanAbort{Abort::Reason::SelectorInvalid};
        }
    };
}

ErrorRegister CanNode::errorRegister() const
{
    return errorRegister_;
}

void CanNode::setErrorRegister(ErrorRegister flags)
{
    errorRegister_ = flags;
}

void CanNode::issueEmergency(Emergency::Type emergencyType, uint64_t deviceError)
{
    media_->writeFrame(Emergency::createFrame(nodeId_, emergencyType, errorRegister(), deviceError));
}

void CanNode::run()
{
    while (true) {
        auto frame = CanFrame::create();
        if (!media_->readFrame(frame)) break;

        try {
            NetworkCommand command{frame};
            if (command->isValid()) {
                if (command->targetId() == nodeId_ || command->targetId() == 0) {
                    handleNodeCommand(command->specifier());
                    continue;
                }
            }
        }
        catch (Exception &ex) {
            ferr() << "NMT failed: " << ex->message() << nl;
        }

        try {
            ServiceRequest request{frame};
            if (request->isValid()) {
                if (request->serverId() != nodeId_) continue;

                #if 0 // TODO
                if (!(
                    nodeState() == NodeState::PreOperational ||
                    nodeState() == NodeState::Operational
                ))
                    continue;
                #endif

                switch (request->specifier()) {
                    case ServiceRequest::Specifier::Write: {
                        handleWriteRequest(frame);
                        break;
                    }
                    case ServiceRequest::Specifier::WriteSegment: {
                        handleWriteSegmentRequest(frame);
                        break;
                    }
                    case ServiceRequest::Specifier::Read: {
                        handleReadRequest(frame);
                        break;
                    }
                    case ServiceRequest::Specifier::ReadSegment: {
                        handleReadSegmentRequest(frame);
                        break;
                    }
                    case ServiceRequest::Specifier::BlockWrite: {
                        handleBlockWriteRequest(frame);
                        break;
                    }
                    case ServiceRequest::Specifier::BlockRead: {
                        handleBlockReadRequest(frame);
                        break;
                    }
                    default: ;
                };

                continue;
            }
        }
        catch (Exception &ex) {
            ferr() << "SDO failed: " << ex->message() << nl;
        }

        if (eventFeed_) {
            if (nodeState() != NodeState::Operational) continue;
            eventFeed_->feedFrame(frame);
        }
    }
}

void CanNode::handleNodeCommand(NetworkCommand::Specifier commandSpecifier)
{
    switch (commandSpecifier) {
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

void CanNode::handleWriteRequest(const CanFrame *head)
{
    if (!WriteRequest{head}) return;

    Selector selector = WriteRequest{head}->selector();

    try {
        if (WriteRequest{head}->hasExpeditedData())
        {
            writeServiceData(+selector, WriteRequest{head}->expeditedData());

            writeHead_ = nullptr;
        }
        else {
            if (WriteRequest{head}->totalDataCount() > uint32_t(buffer_->count()))
                throw CanAbort{Abort::Reason::OutOfMemory};

            writeHead_ = head;
            fill_ = 0;
        }

        readHead_ = nullptr;

        media_->writeFrame(
            WriteReply::createFrame(nodeId_, selector)
        );
    }
    catch (CanAbort &abort) {
        canAbort(selector, abort->reason());
    }
}

void CanNode::handleWriteSegmentRequest(const CanFrame *tail)
{
    if (!writeHead_) return;

    try {
        if (!WriteSegmentRequest{tail})
            throw CanAbort{Abort::Reason::GeneralError};

        auto replyFrame = WriteSegmentReply::createFrame(nodeId_, fill_);

        if (WriteSegmentRequest{tail}->toggle() != WriteSegmentReply{replyFrame}->toggle())
            throw CanAbort{Abort::Reason::ToggleBitInvalid};

        {
            const int segmentDataCount = WriteSegmentRequest{tail}->dataCount();
            if (fill_ + segmentDataCount > buffer_->count())
                throw CanAbort(Abort::Reason::OutOfMemory);

            for (int i = 0; i < segmentDataCount; ++i) {
                mutate(buffer_)->byteAt(fill_) = WriteSegmentRequest{tail}->dataAt(i);
                ++fill_;
            }
        }

        if (WriteSegmentRequest{tail}->isLast()) {
            String data = buffer_->copy(0, fill_);
            writeServiceData(+WriteRequest{writeHead_}->selector(), data);
            writeHead_ = nullptr;
        }

        media_->writeFrame(replyFrame);
    }
    catch (CanAbort &abort) {
        canAbort(WriteRequest{writeHead_}->selector(), abort->reason());
    }
}

void CanNode::handleReadRequest(const CanFrame *head)
{
    if (!ReadRequest{head}) return;

    Selector selector = ReadRequest{head}->selector();

    try {
        String data = readServiceData(+selector);

        auto frame = ReadReply::createFrame(nodeId_, selector, data);

        if (ReadReply{frame}->hasExpeditedData())
        {
            readHead_ = nullptr;
        }
        else {
            if (data->count() > buffer_->count())
                throw CanAbort{Abort::Reason::OutOfMemory};

            readHead_ = head;
            mutate(buffer_)->write(data);
            fill_ = data->count();
            offset_ = 0;
        }

        writeHead_ = nullptr;

        media_->writeFrame(frame);
    }
    catch (CanAbort &abort) {
        canAbort(selector, abort->reason());
    }
}

void CanNode::handleReadSegmentRequest(const CanFrame *tail)
{
    if (!readHead_) return;

    try {
        if (!ReadSegmentRequest{tail})
            throw CanAbort{Abort::Reason::GeneralError};

        auto frame = ReadSegmentReply::createFrame(nodeId_, buffer_, fill_, offset_);

        if (ReadSegmentRequest{tail}->toggle() != ReadSegmentReply{frame}->toggle())
            throw CanAbort{Abort::Reason::ToggleBitInvalid};

        offset_ += ReadSegmentReply{frame}->dataCount();

        media_->writeFrame(frame);
        if (ReadSegmentReply{frame}->isLast()) readHead_ = nullptr;
    }
    catch (CanAbort &abort) {
        canAbort(ReadRequest{readHead_}->selector(), abort->reason());
    }
}

void CanNode::handleBlockWriteRequest(const CanFrame *head)
{
    if (!BlockWriteInitRequest{head}) return;

    Selector selector = BlockWriteInitRequest{head}->selector();

    try {
        uint32_t totalLeft = BlockWriteInitRequest{head}->totalDataCount();
        bool crcSupport = BlockWriteInitRequest{head}->crcSupport();

        if (totalLeft > uint32_t(buffer_->count()))
            throw CanAbort{Abort::Reason::OutOfMemory};

        if (totalLeft == 0) totalLeft = buffer_->count();

        int blockSize = 0x7F;
        if (0 < totalLeft && totalLeft < 7 * 0x7F)
            blockSize = totalLeft / 7 + (totalLeft % 7 > 0);

        media_->writeFrame(
            BlockWriteInitReply::createFrame(nodeId_, selector, blockSize)
        );

        fill_ = 0;

        for (bool go = true; go;)
        {
            int segmentsCount = 0;

            while (segmentsCount < blockSize && go)
            {
                auto frame = getNextRequest();

                if (!BlockWriteSegmentRequest{frame})
                    throw CanAbort{Abort::Reason::GeneralError};

                if (BlockWriteSegmentRequest{frame}->sequenceNumber() > segmentsCount + 1) break;

                mutate(buffer_)->write(BlockWriteSegmentRequest{frame}->segmentData(), fill_ + 7 * segmentsCount, fill_ + 7 * (segmentsCount + 1));
                ++segmentsCount;

                if (BlockWriteSegmentRequest{frame}->isLast()) go = false;
            }

            if (segmentsCount == blockSize || !go) {
                totalLeft -= 7 * segmentsCount;
                fill_ += 7 * segmentsCount;
            }

            if (0 < totalLeft && totalLeft < 7 * 0x7F)
                blockSize = totalLeft / 7 + (totalLeft % 7 > 0);

            if (totalLeft <= 0 && go) throw CanAbort{Abort::Reason::GeneralError};

            media_->writeFrame(
                BlockWriteAckReply::createFrame(nodeId_, segmentsCount, blockSize)
            );
        }

        auto frame = getNextRequest();

        if (AbortRequest{frame})
            throw CanPeerAbort{AbortRequest{frame}->reason()};

        if (!BlockWriteEndRequest{frame})
            throw CanAbort{Abort::Reason::GeneralError};

        fill_ -= 7 - BlockWriteEndRequest{frame}->lastSegmentDataCount();
        String data = buffer_->copy(0, fill_);
        if (crcSupport) {
            uint16_t checkSum = crc16(data);
            if (checkSum != BlockWriteEndRequest{frame}->crc())
                throw CanAbort{Abort::Reason::CrcError};
        }

        writeServiceData(+selector, data);

        media_->writeFrame(
            BlockWriteEndReply::createFrame(nodeId_)
        );
    }
    catch (CanAbort &abort) {
        canAbort(selector, abort->reason());
    }
}

void CanNode::handleBlockReadRequest(const CanFrame *head)
{
    if (!BlockReadInitRequest{head}) return;

    Selector selector = BlockReadInitRequest{head}->selector();

    try {
        String data = readServiceData(+selector);

        if (data->count() < BlockReadInitRequest{head}->switchThreshold()) {
            handleReadRequest(
                ReadRequest::createFrame(
                    BlockReadInitRequest{head}->serverId(),
                    BlockReadInitRequest{head}->selector()
                )
            );

            return;
        }

        media_->writeFrame(
            BlockReadInitReply::createFrame(nodeId_, selector, data)
        );

        {
            auto frame = getNextRequest();
            if (!BlockReadStartRequest{frame})
                throw CanAbort{Abort::Reason::GeneralError};
        }

        int blockSize = BlockReadInitRequest{head}->blockSize();
        int offset = 0;
        int seqNum = 1;

        for (bool go = true; go;)
        {
            for (;seqNum <= blockSize && go; ++seqNum) {
                auto frame = BlockReadSegmentReply::createFrame(nodeId_, data, offset + 7 * (seqNum - 1), seqNum);
                media_->writeFrame(frame);
                go = !BlockReadSegmentReply{frame}->isLast();
            }

            auto frame = getNextRequest();
            if (!BlockReadAckRequest{frame})
                throw CanAbort{Abort::Reason::GeneralError};

            if (BlockReadAckRequest{frame}->sequenceNumber() == blockSize || !go) {
                offset += 7 * (seqNum - 1);
                seqNum = 1;
            }
            else
                seqNum = BlockReadAckRequest{frame}->sequenceNumber() + 1;

            blockSize = BlockReadAckRequest{frame}->newBlockSize();
        }

        media_->writeFrame(
            BlockReadEndReply::createFrame(nodeId_, data, BlockReadInitRequest{head}->crcSupport())
        );

        auto frame = getNextRequest();

        if (AbortRequest{frame})
            throw CanPeerAbort{AbortRequest{frame}->reason()};

        if (!BlockReadEndRequest{frame})
            throw CanAbort{Abort::Reason::GeneralError};
    }
    catch (CanAbort &abort) {
        canAbort(selector, abort->reason());
    }
}

Ref<CanFrame> CanNode::getNextRequest()
{
    auto frame = CanFrame::create();

    while (true) {
        if (!media_->waitFrame(timeout_))
            throw CanAbort{Abort::Reason::ServiceDataTimeout};

        if (!media_->readFrame(frame)) throw CanDisconnect{};

        if (!ServiceRequest{frame}) continue;
        if (ServiceRequest{frame}->serverId() != nodeId_) continue;

        break;
    }

    return frame;
}

void CanNode::canAbort(Selector selector, Abort::Reason reason)
{
    media_->writeFrame(
        AbortReply::createFrame(nodeId_, selector, reason)
    );

    writeHead_ = nullptr;
    readHead_ = nullptr;
}

}} // namespace cc::can
