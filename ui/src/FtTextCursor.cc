/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

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

bool FtTextCursor::isValid() const
{
    return ftTextRun_;
}

int FtTextCursor::move(int steps)
{
    if (!isValid()) return false;
    return ftTextRun_->moveTextCursor(this, steps);
}

Ref<TextCursor> FtTextCursor::copy() const
{
    return Object::create<FtTextCursor>(this);
}

}} // namespace cc::ui
