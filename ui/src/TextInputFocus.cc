/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/TextInputFocus>

namespace cc {
namespace ui {

Ref<TextInputFocus> TextInputFocus::create(View *target)
{
    return new TextInputFocus(target);
}

TextInputFocus::TextInputFocus(View *target):
    target_(target)
{
    if (isValid()) {
        Rect inputArea = target->textInputArea();
        inputArea = Rect{
            target->mapToGlobal(inputArea->pos()),
            inputArea->size()
        };

        Application::instance()->startTextInput(
            target->window(),
            inputArea
        );

        Application::instance()->textInputFocus = this;

        target->onTextInputStarted();
    }
}

TextInputFocus::~TextInputFocus()
{
    if (isValid()) {
        Application::instance()->stopTextInput();
        Application::instance()->textInputFocus = Ref<const TextInputFocus>{};
        target_->onTextInputStopped();
    }
}

bool TextInputFocus::isValid() const
{
    return target_ && target_->window() && target_->hasTextInput();
}

View *TextInputFocus::target() const
{
    return target_;
}

}} // namespace cc::ui
