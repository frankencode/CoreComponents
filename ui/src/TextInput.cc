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

TextInput::TextInput(View *parent, const String &initialText):
    Control(parent),
    text{initialText}
{
    inheritPaper();

    font->bind([=]{ return app()->defaultFont(); });

    if (text()->count() > 0) {
        selection = Range { 0, text()->count() };
        nextTextCursorOffset_ = text()->count();
    }

    textRun->bind([=]{ return TextRun::create(text(), font()); });
    textCursor->bind([=]{ return textRun()->getTextCursor(nextTextCursorOffset_); });

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
    Range s = selection();
    if (!s) s = textCursor()->byteOffset();
    paste(s, chunk);
}

bool TextInput::onKeyPressed(const KeyEvent *event)
{
    if (
        (+(event->modifiers() & KeyModifier::Control)) &&
        event->keyCode() == KeyCode::Key_A
    ) {
        selection = Range { 0, text()->count() };
        textCursor()->step(text()->count());
        startBlink();
    }
    else if (
        event->scanCode() == ScanCode::Key_Left ||
        event->scanCode() == ScanCode::Key_Home
    )
    {
        bool shrinkSelection =
            (+(event->modifiers() & KeyModifier::Shift)) &&
            selection() &&
            selection()->count() > 1 &&
            selection()->i1() == textCursor()->byteOffset();

        bool growSelection =
            (+(event->modifiers() & KeyModifier::Shift)) &&
            selection() &&
            selection()->i0() == textCursor()->byteOffset();

        bool createSelection =
            (+(event->modifiers() & KeyModifier::Shift)) &&
            !selection();

        int newSelectionEnd = textCursor()->byteOffset();

        textCursor()->step((event->scanCode() == ScanCode::Key_Home) ? -text()->count() : -1);

        if (shrinkSelection)
            selection = Range { selection()->i0(), textCursor()->byteOffset() };
        else if (growSelection)
            selection = Range { textCursor()->byteOffset(), selection()->i1() };
        else if (createSelection && newSelectionEnd != textCursor()->byteOffset())
            selection = Range { textCursor()->byteOffset(), newSelectionEnd };
        else
            selection = Range{};

        startBlink();
        update();
    }
    else if (
        event->scanCode() == ScanCode::Key_Right ||
        event->scanCode() == ScanCode::Key_End
    )
    {
        bool shrinkSelection =
            (+(event->modifiers() & KeyModifier::Shift)) &&
            selection() &&
            selection()->count() > 1 &&
            selection()->i0() == textCursor()->byteOffset();

        bool growSelection =
            (+(event->modifiers() & KeyModifier::Shift)) &&
            selection() &&
            selection()->i1() == textCursor()->byteOffset();

        bool createSelection =
            (+(event->modifiers() & KeyModifier::Shift)) &&
            !selection();

        int newSelectionStart = textCursor()->byteOffset();

        textCursor()->step((event->scanCode() == ScanCode::Key_End) ? text()->count() : 1);

        if (shrinkSelection)
            selection = Range { textCursor()->byteOffset(), selection()->i1() };
        else if (growSelection)
            selection = Range { selection()->i0(), textCursor()->byteOffset() };
        else if (createSelection && newSelectionStart != textCursor()->byteOffset())
            selection = Range { newSelectionStart, textCursor()->byteOffset() };
        else
            selection = Range{};

        startBlink();
        update();
    }
    else if (event->scanCode() == ScanCode::Key_Backspace)
    {
        Range s = selection();
        if (!s) s = Range { textCursor()->byteOffset() - 1, textCursor()->byteOffset() };
        paste(s, String{});
    }
    else if (event->scanCode() == ScanCode::Key_Delete)
    {
        Range s = selection();
        if (!s) s = Range { textCursor()->byteOffset(), textCursor()->byteOffset() + 1 };
        paste(s, String{});
    }
    else if (
        (+(event->modifiers() & KeyModifier::Control)) && (
            event->scanCode() == ScanCode::Key_Insert ||
            event->keyCode() == KeyCode::Key_C
        )
    )
    {
        if (selection()) {
            app()->setClipboardText(
                text()->copy(selection()->i0(), selection()->i1())
            );
        }
    }
    else if (
        event->scanCode() == ScanCode::Key_Insert || (
            (+(event->modifiers() & KeyModifier::Control)) &&
            event->keyCode() == KeyCode::Key_V
        )
    ) {
        String chunk = app()->getClipboardText();
        if (chunk) paste(chunk);
    }

    return true;
}

void TextInput::paste(const String &chunk)
{
    Range s = selection();
    if (!s) s = textCursor()->byteOffset();
    paste(s, chunk);
}

void TextInput::paste(Range range, const String &chunk)
{
    if (!
        (0 <= range->i0() && range->i1() <= text()->count())
    )
        return;

    selection = Range{};

    nextTextCursorOffset_ = range->i0() + chunk->count();

    text =
        text()->copy(0, range->i0()) +
        chunk +
        text()->copy(range->i1(), text()->count());

    startBlink();
}

void TextInput::paint()
{
    Painter p(this);

    p->setSource(style()->theme()->primaryTextColor());

    if (selection() && focus()) {
        int s0 = selection()->i0();
        int s1 = selection()->i1();
        p->showTextRun(
            textPos(), textRun(),
            Painter::GetColor{},
            [=](int byteOffset) -> Color {
                return (s0 <= byteOffset && byteOffset < s1) ? theme()->textSelectionColor() : Color{};
            }
        );
    }
    else {
        p->showTextRun(textPos(), textRun());
    }

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
