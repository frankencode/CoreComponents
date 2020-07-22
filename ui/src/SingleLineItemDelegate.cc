/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SingleLineItemDelegate>
#include <cc/ui/Picture>
#include <cc/ui/Label>

namespace cc {
namespace ui {

SingleLineItemDelegate::Instance::Instance(const String &initialText, Visual *initialIcon):
    icon{initialIcon},
    text{initialText}
{
    build >>[=]{
        auto height = [=]{
            return std::ceil(icon() ? dp(56) : dp(48));
        };

        Picture picture{icon()};
        picture->visual <<[=]{ return icon(); };
        picture->pos <<[=]{
            return Point{
                std::round(dp(16)),
                std::round((height() - picture->size()[1]) / 2)
            };
        };
        (*this) << picture;

        Label label{text()};
        label->text <<[=]{ return text(); };
        label->anchorTextCenterLeftTo([=]{
            if (!icon()) return Point{ dp(16), height() / 2 };
            return picture->centerRight() + Step{ picture->size()[1] >= dp(32) ? dp(16) : dp(32), 0 };
        });
        (*this) << label;

        preferredSize_ <<[=]{
            return Size{
                std::ceil(label->right() + dp(16)),
                height()
            };
        };

        size <<[=]{
            return Size{
                this->parent()->size()[0],
                preferredSize_()[1]
            };
        };
    };
}

Size SingleLineItemDelegate::Instance::preferredSize() const
{
    return preferredSize_();
}

Size SingleLineItemDelegate::Instance::minSize() const
{
    return preferredSize_();
}

}} // namespace cc::ui
