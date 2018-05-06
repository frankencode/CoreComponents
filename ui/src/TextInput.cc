/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/ui/TextRun>
#include <cc/ui/Timer>
#include <cc/ui/TextInput>

namespace cc {
namespace ui {

TextInput::TextInput(View *parent):
    Control(parent)
{
    inheritPaper();

    font->bind([=]{ return app()->defaultFont(); });

    textRun->bind([=]{ return TextRun::create(text(), font()); });
    textCursor->bind([=]{ return textRun()->getTextCursor(textCursorOffset()); });

    size->bind([=]{
        return Size {
            parent->size()[0],
            textRun()->size()[1] + 2 * margin()[1]
        };
    });

    cursor->bind([=]{ return Cursor::create(focus() ? CursorShape::IBeam : CursorShape::Hand); });

    focus->connect([=]{
        if (focus()) startBlink();
        else stopBlink();
    });

    timer_ = Timer::create(0.5);
    timer_->triggered->connect([=]{
        textCursorVisible = !textCursorVisible();
    });
}

TextInput::~TextInput()
{}

void TextInput::startBlink()
{
    textCursorVisible = false;
    timer_->startIn(0);
}

void TextInput::stopBlink()
{
    timer_->stop();
    textCursorVisible = false;
}

Point TextInput::textPos() const
{
    double a = std::ceil(font()->getMetrics()->ascender());
    return margin() + Point { 0, a };
}

bool TextInput::onPointerClicked(const PointerEvent *event)
{
    app()->focusControl = this;
    return true;
}

void TextInput::onTextEdited(const String &chunk, int start, int length)
{
    CC_INSPECT(chunk);
}

void TextInput::onTextInput(const String &chunk)
{
    text =
        text()->copy(0, textCursorOffset()) +
        chunk +
        text()->copy(textCursorOffset(), text()->count());

    textCursorOffset += chunk->count();

    startBlink();
}

bool TextInput::onKeyPressed(const KeyEvent *event)
{
    if (event->scanCode() == ScanCode::Key_Left)
    {
        // CC_INSPECT(event);
        textCursor()->step(-1);
        startBlink();
        update();
    }
    else if (event->scanCode() == ScanCode::Key_Right)
    {
        textCursor()->step(1);
        startBlink();
        update();
    }

    return true;
}

void TextInput::paint()
{
    Painter p(this);
    p->setSource(style()->theme()->primaryTextColor());
    p->showTextRun(textPos(), textRun());
    if ((focus() || pressed()) && textCursorVisible()) {
        p->setSource(theme()->textCursorColor());
        p->rectangle(
            textPos() + textCursor()->posA(),
            Size { dp(1), (textCursor()->posB() - textCursor()->posA())[1] }
        );
        p->fill();
    }
}

}} // namespace cc::ui
