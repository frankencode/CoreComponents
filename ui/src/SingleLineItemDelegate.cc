/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/ui/Picture>
#include <cc/ui/Label>
#include <cc/ui/SingleLineItemDelegate>

namespace cc {
namespace ui {

SingleLineItemDelegate::SingleLineItemDelegate(View *parent, const String &initialText, Visual *initialIcon):
    View{parent},
    icon{initialIcon},
    text{initialText}
{}

void SingleLineItemDelegate::init()
{
    size->bind([=]{
        return Size{
            parent()->size()[0],
            icon() ? dp(56) : dp(48)
        };
    });

    CC_INSPECT(size());

    auto picture = add<Picture>(icon());
    picture->visual->bind([=]{ return icon(); });
    picture->pos->bind([=]{
        return Point{
            dp(16),
            (size()[1] - picture->size()[1]) / 2
        };
    });

    auto label = add<Label>(text());
    label->text->bind([=]{ return text(); });
    label->anchorTextCenterLeftTo([=]{
        if (!icon()) return Point{ dp(16), size()[1] / 2 };
        return picture->centerRight() + Step{ picture->size()[1] >= dp(32) ? dp(16) : dp(32), 0 };
    });
}

}} // namespace cc::ui
