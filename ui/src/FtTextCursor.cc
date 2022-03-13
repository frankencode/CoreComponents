/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtTextCursor>
#include <cc/ui/FtTextRun>

namespace cc::ui {

FtTextCursor FtTextCursor::State::self() const
{
    return Object::alias<FtTextCursor>(this);
}

TextCursor FtTextCursor::State::copy() const
{
    return FtTextCursor{new State{*this}};
}

void FtTextCursor::State::assign(const TextCursor &other)
{
    if (!other) return;

    State &otherState = other.as<FtTextCursor>().me();

    if (textRun_ != otherState.textRun_)
        textRun_ = otherState.textRun_;

    as<TextCursor::State>() = otherState;
}

int FtTextCursor::State::step(int steps)
{
    FtTextCursor movingCursor = self();
    return textRun_.moveTextCursor(movingCursor, steps);
}

int FtTextCursor::State::lineStep(int steps)
{
    int stepsMoved = 0;

    if (steps != 0) {
        FtTextCursor movingCursor = self();
        int offsetSaved = offset_;
        for (; steps < 0; ++steps) {
            FtTextCursor aboveCursor = textRun_.getNearestTextCursorAbove(movingCursor).as<FtTextCursor>();
            if (aboveCursor.offset() == offsetSaved) break;
            movingCursor = aboveCursor;
            ++stepsMoved;
        }
        for (; steps > 0; --steps) {
            FtTextCursor belowCursor = textRun_.getNearestTextCursorBelow(movingCursor).as<FtTextCursor>();
            if (belowCursor.offset() == offsetSaved) break;
            movingCursor = belowCursor;
            ++stepsMoved;
        }
        posA_ = movingCursor.me().posA_;
        posB_ = movingCursor.me().posB_;
        offset_ = movingCursor.me().offset_;
        runIndex_ = movingCursor.me().runIndex_;
        clusterIndex_ = movingCursor.me().clusterIndex_;
        glyphIndex_ = movingCursor.me().glyphIndex_;
    }

    return stepsMoved;
}

} // namespace cc::ui
