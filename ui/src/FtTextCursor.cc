/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/FtTextRun>
#include <cc/ui/FtTextCursor>

namespace cc {
namespace ui {

FtTextCursor::FtTextCursor(const FtTextRun *ftTextRun):
    ftTextRun_(ftTextRun),
    byteOffset_(0),
    runIndex_(0),
    clusterIndex_(0),
    glyphIndex_(0)
{}

FtTextCursor::FtTextCursor(const FtTextCursor *other):
    ftTextRun_(other->ftTextRun_),
    posA_(other->posA_),
    posB_(other->posB_),
    byteOffset_(other->byteOffset_),
    runIndex_(other->runIndex_),
    clusterIndex_(other->clusterIndex_),
    glyphIndex_(other->glyphIndex_)
{}

Point FtTextCursor::advance() const
{
    return ftTextRun_ ? ftTextRun_->advance(this) : Point{};
}

bool FtTextCursor::isValid() const
{
    return ftTextRun_;
}

int FtTextCursor::step(int steps)
{
    if (!isValid()) return false;
    return ftTextRun_->moveTextCursor(this, steps);
}

int FtTextCursor::lineStep(int steps)
{
    int stepsMoved = 0;
    if (steps != 0) {
        Ref<FtTextCursor> movingCursor = this;
        for (; steps < 0; ++steps) {
            Ref<FtTextCursor> aboveCursor = ftTextRun_->getNearestTextCursorAbove(movingCursor);
            if (aboveCursor->byteOffset() == byteOffset()) break;
            movingCursor = aboveCursor;
            ++stepsMoved;
        }
        for (; steps > 0; --steps) {
            Ref<FtTextCursor> belowCursor = ftTextRun_->getNearestTextCursorBelow(movingCursor);
            if (belowCursor->byteOffset() == byteOffset()) break;
            movingCursor = belowCursor;
            ++stepsMoved;
        }
        posA_ = movingCursor->posA_;
        posB_ = movingCursor->posB_;
        byteOffset_ = movingCursor->byteOffset_;
        runIndex_ = movingCursor->runIndex_;
        clusterIndex_ = movingCursor->clusterIndex_;
        glyphIndex_ = movingCursor->glyphIndex_;
    }
    return stepsMoved;
}

Ref<TextCursor> FtTextCursor::copy() const
{
    return ftCopy();
}

Ref<FtTextCursor> FtTextCursor::ftCopy() const
{
    return Object::create<FtTextCursor>(this);
}

String FtTextCursor::toString() const
{
    return Format()
        << "FtTextCursor {" << nl
        << "  posA: " << posA_ << nl
        << "  posB: " << posB_ << nl
        << "  advance: " << advance() << nl
        << "  byteOffset: " << byteOffset_ << nl
        << "  isValid: " << isValid() << nl
        << "  runIndex: " << runIndex_ << nl
        << "  clusterIndex: " << clusterIndex_ << nl
        << "  glyphIndex: " << glyphIndex_ << nl
        << "}";
}

}} // namespace cc::ui
