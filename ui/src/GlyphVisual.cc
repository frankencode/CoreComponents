/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Utf8Sink>
#include <cc/ui/Painter>
#include <cc/ui/GlyphRun>
#include <cc/ui/GlyphVisual>

namespace cc {
namespace ui {

GlyphVisual::GlyphVisual(uchar_t ch):
    s_(String::create(4))
{
    auto sink = Utf8Sink::open(mutate(s_));
    sink->write(ch);
    mutate(s_)->truncate(sink->byteSink()->currentOffset());

    glyphRun_->bind([=]{ return GlyphRun::typeset(s_, font()); });
    preferredSize_->bind([=]{ return Size{ glyphRun_()->size()[0], glyphRun_()->maxAscender() }; });
    size->bind([=]{ return preferredSize_(); });
}

void GlyphVisual::paint(Painter &p)
{
    p->save();
    p->setSource(ink());
    p->showGlyphRun(
        size() / 2 +
        Size{
            -preferredSize_()[0] / 2,
            +preferredSize_()[1] / 2
        },
        glyphRun_()
    );
    p->restore();
}

}} // namespace cc::ui
