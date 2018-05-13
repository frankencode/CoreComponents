/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtTextCursor>
#include <cc/ui/TextRun>
#include <cc/ui/Timer>
#include <cc/ui/Editor>
#include <cc/ui/TextInput>

namespace cc {
namespace ui {

TextInput::TextInput(View *parent, const String &initialText):
    Control(parent),
    editor_(Object::create<Editor>(initialText))
{
    inheritPaper();

    font->bind([=]{ return app()->defaultFont(); });

    textRun->bind([=]{
        auto run = TextRun::create();
        if (imeChunks()) {
            for (const String &chunk: imeChunks()) {
                if (chunk->count() > 0)
                    run->append(chunk, font());
            }
        }
        else
            run->append(text(), font());
        return run;
    });

    if (text()->count() > 0)
        selection = Range { 0, text()->count() };

    textCursor = textRun()->getTextCursor(text()->count());

    textRun->connect([=]{
        if (textCursor())
            textCursor = textRun()->getTextCursor(textCursor()->byteOffset());
    });

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

String TextInput::text() const
{
    return editor_->text();
}

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
    if (!focus()) {
        app()->focusControl = this;
        return true;
    }

    Ref<const MouseEvent> mouseEvent = event;
    if (mouseEvent) {
        if (mouseEvent->clickCount() == 2) {
            selection = Range { 0, text()->count() };
            textCursor()->step(text()->count());
            startBlink();
        }
    }

    return true;
}

bool TextInput::onPointerPressed(const PointerEvent *event)
{
    if (!focus()) return false;

    if (shiftKey_) return onPointerMoved(event);

    selection = Range{};
    imeChunks = nullptr;
    textCursor = textRun()->getNearestTextCursor(event->pos() - textPos());

    startBlink();

    return true;
}

bool TextInput::onPointerMoved(const PointerEvent *event)
{
    if (!focus()) return false;

    Ref<TextCursor> newTextCursor = textRun()->getNearestTextCursor(event->pos() - textPos());

    if (newTextCursor->byteOffset() == textCursor()->byteOffset())
        return true;

    if (!selection()) {
        selection = Range { newTextCursor->byteOffset(), textCursor()->byteOffset() };
    }
    else {
        Range newSelection;
        if (textCursor()->byteOffset() == selection()->i0())
            newSelection = Range { newTextCursor->byteOffset(), selection()->i1() };
        else if (textCursor()->byteOffset() == selection()->i1())
            newSelection = Range { selection()->i0(), newTextCursor->byteOffset() };
        if (newSelection->count() > 0)
            selection = newSelection;
        else
            selection = Range{};
    }

    textCursor = newTextCursor;

    return true;
}

Rect TextInput::textInputArea() const
{
    double cx = (textCursor()) ? textCursor()->posA()[0] : 0.;

    Point origin { textPos()[0] + cx, 0 };

    return Rect {
        origin,
        Size {
            size()[0] - origin[0],
            size()[1]
        }
    };
}

void TextInput::onTextEdited(const String &chunk, int start, int length)
{
    imeChunks =
        StringList::create()
            << text()->copy(0, textCursor()->byteOffset())
            << chunk
            << text()->copy(textCursor()->byteOffset(), text()->count());
}

void TextInput::onTextInput(const String &chunk)
{
    Range s = selection();
    if (!s) s = Range { textCursor()->byteOffset() };
    paste(s, chunk);
}

bool TextInput::onKeyPressed(const KeyEvent *event)
{
    imeChunks = nullptr;

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
        if (!s) {
            int i1 = textCursor()->byteOffset();
            if (textCursor()->step(-1))
                s = Range { textCursor()->byteOffset(), i1 };
        }
        if (s) paste(s, String{});
    }
    else if (event->scanCode() == ScanCode::Key_Delete)
    {
        Range s = selection();
        if (!s) {
            int i0 = textCursor()->byteOffset();
            if (textCursor()->step(1))
            s = Range { i0, textCursor()->byteOffset() };
        }
        if (s) paste(s, String{});
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
    else if (
        (+(event->modifiers() & KeyModifier::Control)) &&
        event->scanCode() == ScanCode::Key_Y
    ) {
        Range range;
        if (+(event->modifiers() & KeyModifier::Shift)) {
            if (editor_->canRedo()) textCursor = nullptr;
            range = editor_->redo();
        }
        else {
            if (editor_->canUndo()) textCursor = nullptr;
            range = editor_->undo();
        }

        if (range) {
            textCursor = textRun()->getTextCursor(range->i1());
            startBlink();
        }
    }
    else if (
        event->scanCode() == ScanCode::Key_LeftShift ||
        event->scanCode() == ScanCode::Key_RightShift
    )
        shiftKey_ = true;

    return true;
}

bool TextInput::onKeyReleased(const KeyEvent *event)
{
    if (
        event->scanCode() == ScanCode::Key_LeftShift ||
        event->scanCode() == ScanCode::Key_RightShift
    )
        shiftKey_ = false;

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
    if (! (0 <= range->i0() && range->i1() <= text()->count()) )
        return;

    selection = Range{};
    textCursor = nullptr;
    imeChunks = nullptr;

    editor_->paste(range, chunk);

    textCursor = textRun()->getTextCursor(range->i0() + chunk->count());
    startBlink();
}

void TextInput::paint()
{
    Painter p(this);

    p->setSource(theme()->primaryTextColor());

    if (selection() && focus()) {
        int s0 = selection()->i0();
        int s1 = selection()->i1();
        p->showTextRun(
            textPos(), textRun(),
            [=](int byteOffset) -> Color {
                return (s0 <= byteOffset && byteOffset < s1) ? theme()->textSelectionInk() : Color{};
            },
            [=](int byteOffset) -> Color {
                return (s0 <= byteOffset && byteOffset < s1) ? theme()->textSelectionPaper() : Color{};
            }
        );
    }
    else {
        p->showTextRun(textPos(), textRun());
    }

    if ((focus() || pressed()) && textCursorVisible() && textCursor()) {
        p->setSource(theme()->textCursorColor());
        p->rectangle(
            textPos() + textCursor()->posA(),
            Size { theme()->textCursorWidth(), (textCursor()->posB() - textCursor()->posA())[1] }
        );
        p->fill();
    }
}

}} // namespace cc::ui
