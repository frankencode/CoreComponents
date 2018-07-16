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
    auto height = [=]{
        return std::ceil(icon() ? dp(56) : dp(48));
    };

    auto picture = add<Picture>(icon());
    picture->visual->bind([=]{ return icon(); });
    picture->pos->bind([=]{
        return Point{
            std::round(dp(16)),
            std::round((height() - picture->size()[1]) / 2)
        };
    });

    auto label = add<Label>(text());
    label->text->bind([=]{ return text(); });
    label->anchorTextCenterLeftTo([=]{
        if (!icon()) return Point{ dp(16), height() / 2 };
        return picture->centerRight() + Step{ picture->size()[1] >= dp(32) ? dp(16) : dp(32), 0 };
    });

    preferredSize_->bind([=]{
        return Size{
            std::ceil(label->right() + dp(16)),
            height()
        };
    });

    size->bind([=]{
        return Size{
            parent()->size()[0],
            preferredSize_()[1]
        };
    });
}

Size SingleLineItemDelegate::preferredSize() const
{
    return preferredSize_();
}

Size SingleLineItemDelegate::minSize() const
{
    return preferredSize_();
}

}} // namespace cc::ui
