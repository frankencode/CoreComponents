/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextInput>
#include <cc/ui/Application>
#include <cc/ui/TextRun>
#include <cc/ui/LineEditor>

namespace cc {
namespace ui {

TextInput::Instance::Instance(const String &initialText)
{
    build >>[=]{
        editor_ = createEditor(initialText);
        inheritPaper();

        if (!font()) font <<[=]{ return Application{}->defaultFont(); };

        unwrappedTextRun <<[=]{
            auto run = TextRun::create();
            if (imeChunks()->count() > 0) {
                for (const String &chunk: imeChunks()) {
                    if (chunk->count() > 0)
                        run->append(chunk, font());
                }
            }
            else
                run->append(text(), font());
            return run;
        };

        textRun <<[=]{
            return unwrappedTextRun()->wrap(size()[0]);
        };

        if (text()->count() > 0)
            selection = Range { 0, text()->count() };

        textCursor = textRun()->getTextCursor(text()->count());

        textRun >>[=]{
            if (textCursor())
                textCursor = textRun()->getTextCursor(textCursor()->byteOffset());
        };

        size <<[=]{
            return preferredSize();
        };

        cursor <<[=]{ return Cursor::create(focus() ? CursorShape::IBeam : CursorShape::Hand); };

        focus >>[=]{
            if (focus()) startBlink();
            else stopBlink();
        };

        timer_ = Timer{0.5};
        timer_->timeout >>[=]{
            textCursorVisible = !textCursorVisible();
        };
    };

    paint >>[=]{
        Painter p{this};

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
    };
}

TextInput::Instance::~Instance()
{}

Size TextInput::Instance::preferredSize() const
{
    double h = 0;
    if (textRun()->lineCount() < 2) {
        Ref<const FontMetrics> m = font()->getMetrics();
        h += std::ceil(m->ascender()) - std::floor(m->descender());
    }
    else
        h += textRun()->size()[1];
    return Size{ 280, h };
}

Size TextInput::Instance::minSize() const
{
    return Size{ 0, preferredSize()[1] };
}

Size TextInput::Instance::maxSize() const
{
    return Size{ std::numeric_limits<double>::max(), preferredSize()[1] };
}

Ref<TextEditor> TextInput::Instance::createEditor(const String &initialText)
{
    return Object::create<LineEditor>(initialText);
}

String TextInput::Instance::text() const
{
    return editor_->text();
}

Point TextInput::Instance::textPos() const
{
    return Point { 0, std::ceil(font()->getMetrics()->ascender()) };
}

void TextInput::Instance::startBlink()
{
    textCursorVisible = false;
    timer_->startIn(0);
}

void TextInput::Instance::stopBlink()
{
    timer_->stop();
    textCursorVisible = false;
}

bool TextInput::Instance::onPointerClicked(const PointerEvent *event)
{
    if (!focus()) {
        Application{}->focusControl = this;
        return true;
    }

    const MouseEvent *mouseEvent = Object::cast<const MouseEvent *>(event);
    if (mouseEvent) {
        if (mouseEvent->clickCount() == 2) {
            selection = Range { 0, text()->count() };
            textCursor()->step(text()->count());
            startBlink();
        }
    }

    return true;
}

bool TextInput::Instance::onPointerPressed(const PointerEvent *event)
{
    if (!focus()) return false;

    if (shiftKey_) return onPointerMoved(event);

    selection = Range{};
    imeChunks = nullptr;
    textCursor = textRun()->getNearestTextCursor(event->pos() - textPos());
    startBlink();

    return true;
}

bool TextInput::Instance::onPointerMoved(const PointerEvent *event)
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

Rect TextInput::Instance::textInputArea() const
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

void TextInput::Instance::onTextEdited(const String &chunk, int start, int length)
{
    imeChunks = StringList {
        text()->copy(0, textCursor()->byteOffset()),
        chunk,
        text()->copy(textCursor()->byteOffset(), text()->count())
    };
}

void TextInput::Instance::onTextInput(const String &chunk)
{
    Range s = selection();
    if (!s) s = Range { textCursor()->byteOffset() };
    paste(s, chunk);
}

bool TextInput::Instance::onKeyPressed(const KeyEvent *event)
{
    imeChunks = nullptr;

    if (
        +(event->modifiers() & KeyModifier::Control) &&
        event->keyCode() == KeyCode::Key_A
    ) {
        selection = Range { 0, text()->count() };
        textCursor()->step(text()->count());
        startBlink();
    }
    else if (
        event->scanCode() == ScanCode::Key_Left ||
        event->scanCode() == ScanCode::Key_Right ||
        event->scanCode() == ScanCode::Key_Up ||
        event->scanCode() == ScanCode::Key_Down ||
        event->scanCode() == ScanCode::Key_Home ||
        event->scanCode() == ScanCode::Key_End
    ) {
        int o = textCursor()->byteOffset();

        textCursor()->step(
            -1 * (event->scanCode() == ScanCode::Key_Left)
            +1 * (event->scanCode() == ScanCode::Key_Right)
            -text()->count() * (event->scanCode() == ScanCode::Key_Home)
            +text()->count() * (event->scanCode() == ScanCode::Key_End)
        );

        textCursor()->lineStep(
            -1 * (event->scanCode() == ScanCode::Key_Up)
            +1 * (event->scanCode() == ScanCode::Key_Down)
        );

        int n = textCursor()->byteOffset();

        if (+(event->modifiers() & KeyModifier::Shift))
        {
            if (selection()) {
                int s = selection()->begin();
                int e = selection()->end();

                if (o == s) s = n;
                else if (o == e) e = n;

                if (s != e)
                    selection = Range{s, e};
                else
                    selection = Range{};
            }
            else if (n != o)
                selection = Range{o, n};
        }
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
        +(event->modifiers() & KeyModifier::Control) &&
        event->keyCode() == KeyCode::Key_X
    )
    {
        if (selection()) {
            Application{}->setClipboardText(
                text()->copy(selection()->i0(), selection()->i1())
            );
            paste(selection(), String{});
        }
    }
    else if (
        +(event->modifiers() & KeyModifier::Control) && (
            event->scanCode() == ScanCode::Key_Insert ||
            event->keyCode() == KeyCode::Key_C
        )
    ) {
        if (selection()) {
            Application{}->setClipboardText(
                text()->copy(selection()->i0(), selection()->i1())
            );
        }
    }
    else if (
        event->scanCode() == ScanCode::Key_Insert || (
            +(event->modifiers() & KeyModifier::Control) &&
            event->keyCode() == KeyCode::Key_V
        )
    ) {
        String chunk = Application{}->getClipboardText();
        if (chunk) paste(chunk);
    }
    else if (
        +(event->modifiers() & KeyModifier::Control) &&
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
    ) {
        shiftKey_ = true;
    }

    return InputControl::Instance::onKeyPressed(event);
}

bool TextInput::Instance::onKeyReleased(const KeyEvent *event)
{
    if (
        event->scanCode() == ScanCode::Key_LeftShift ||
        event->scanCode() == ScanCode::Key_RightShift
    )
        shiftKey_ = false;

    return true;
}

void TextInput::Instance::paste(const String &chunk)
{
    Range s = selection();
    if (!s) s = Range { textCursor()->byteOffset() };
    paste(s, chunk);
}

void TextInput::Instance::paste(Range range, const String &chunk)
{
    if (! (0 <= range->i0() && range->i1() <= text()->count()) )
        return;

    selection = Range{};
    textCursor = nullptr;
    imeChunks = nullptr;

    String filteredChunk = chunk;
    mutate(filteredChunk)->replaceInsitu("\n", " ");

    Range newRange = editor_->paste(range, filteredChunk);
    if (newRange) {
        textCursor = textRun()->getTextCursor(newRange->i1());
        startBlink();
    }
}

}} // namespace cc::ui
