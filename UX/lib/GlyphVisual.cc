/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/GlyphVisual>
#include <cc/GlyphRun>
#include <cc/Utf8Sink>

namespace cc {

struct GlyphVisual::State: public Visual::State
{
    static String encode(uint32_t ch)
    {
        String s = String::allocate(4);
        Utf8Sink sink{s};
        sink.write(ch);
        s.truncate(sink.currentOffset());
        return s;
    }

    State(uint32_t ch, const Font &font)
    {
        glyphRun_ = GlyphRun{encode(ch), font};

        size(Size{
            std::ceil(glyphRun_.size()[0]),
            std::ceil(glyphRun_.maxAscender() - 2 * glyphRun_.minDescender())
        });
    }

    void paint(Painter &p) const override
    {
        p.showGlyphRun(
            size() / 2 +
            Step{
                - glyphRun_.size()[0] / 2,
                + glyphRun_.maxAscender() / 2
            },
            glyphRun_
        );
    }

    GlyphRun glyphRun_;
};

GlyphVisual::GlyphVisual(uint32_t ch, const Font &font):
    Visual{new State{ch, font}}
{}

} // namespace cc
