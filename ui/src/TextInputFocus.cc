/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/Control>
#include <cc/ui/TextInputFocus>

namespace cc {
namespace ui {

const TextInputFocus *TextInputFocus::create(Control *target)
{
    return new TextInputFocus(target);
}

TextInputFocus::TextInputFocus(Control *target):
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
        target_->onTextInputStopped();
    }
}

bool TextInputFocus::isValid() const
{
    return target_ && target_->window() && !Application::fin();
}

Control *TextInputFocus::target() const
{
    return target_;
}

}} // namespace cc::ui
