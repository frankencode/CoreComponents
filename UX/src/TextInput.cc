/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TextInput>
#include <cc/TextInputState>
#include <cc/TextLineEditor>
#include <cc/TextRun>
#include <cc/FontMetrics>
#include <cc/Painter>
#include <cc/Application>

namespace cc {

TextInput::State::State():
    State{TextLineEditor{}}
{}

TextInput::State::State(const TextEditor &editor):
    editor_{editor}
{
    paper([this]{ return basePaper(); });

    font([this]{ return style().defaultFont(); });

    unwrappedTextRun([this]{
        TextRun run;
        if (imeChunks().count() > 0) {
            for (const String &chunk: imeChunks()) {
                if (chunk.count() > 0)
                    run.append(chunk, font());
            }
        }
        else
            run.append(text(), font());
        return run;
    });

    textRun([this]{
        return unwrappedTextRun().wrap(size()[0], textAlign());
    });

    if (text().count() > 0)
        selection = Range{0, text().count()};

    textCursor = textRun().getTextCursor(text().count());

    textRun.onChanged([this]{
        if (textCursor()) {
            int offset = textCursor().offset();
            if (offset > textRun().byteCount()) offset = textRun().byteCount();
            textCursor(textRun().getTextCursor(offset));
        }
    });

    size([this]{ return preferredSize(); });

    cursor([this]{ return Cursor{focus() ? CursorShape::IBeam : CursorShape::Hand}; });

    focus.onChanged([this]{
        if (focus()) startBlink();
        else stopBlink();
    });

    timer_.onTimeout([this]{
        textCursorVisible = !textCursorVisible();
    });

    paint([this]
    {
        Painter p{this};

        p.setPen(theme().primaryTextColor());

        if (selection() && focus()) {
            long s0 = selection().i0();
            long s1 = selection().i1();
            p.showTextRun(
                textPos(), textRun(),
                [=](int byteOffset) -> Color {
                    return (s0 <= byteOffset && byteOffset < s1) ? theme().textSelectionColor() : Color{};
                },
                [=](int byteOffset) -> Color {
                    return (s0 <= byteOffset && byteOffset < s1) ? theme().textSelectionPaper() : Color{};
                }
            );
        }
        else {
            p.showTextRun(textPos(), textRun());
        }

        if ((focus() || pressed()) && textCursorVisible() && textCursor()) {
            p.rectangle(
                textPos() + textCursor().posA(),
                Size{theme().textCursorWidth(), (textCursor().posB() - textCursor().posA())[1] }
            );
            p.setPen(theme().textCursorColor());
            p.fill();
        }
    });

    onPointerClicked([this](const PointerEvent &event)
    {
        if (!focus()) {
            grabFocus();
            return true;
        }

        if (event.is<MouseEvent>()) {
            if (event.as<MouseEvent>().clickCount() == 2) {
                selection = Range{0, text().count()};
                textCursor().step(text().count());
                startBlink();
            }
        }

        return true;
    });

    onPointerPressed([this](const PointerEvent &event)
    {
        if (!focus()) return false;

        if (shiftKey_) return onPointerMoved(event);

        selection = Range{};
        imeChunks = List<String>{};
        textCursor = textRun().getNearestTextCursor(event.pos() - textPos());
        startBlink();

        return true;
    });

    onPointerMoved([this](const PointerEvent &event)
    {
        if (!focus()) return false;

        TextCursor newTextCursor = textRun().getNearestTextCursor(event.pos() - textPos());

        if (newTextCursor.offset() == textCursor().offset())
            return true;

        if (!selection()) {
            selection = Range{newTextCursor.offset(), textCursor().offset()};
        }
        else {
            Range newSelection;
            if (textCursor().offset() == selection().i0())
                newSelection = Range{newTextCursor.offset(), selection().i1()};
            else if (textCursor().offset() == selection().i1())
                newSelection = Range{selection().i0(), newTextCursor.offset()};
            if (newSelection.count() > 0)
                selection = newSelection;
            else
                selection = Range{};
        }

        textCursor = newTextCursor;

        return true;
    });

    onKeyPressed([this](const KeyEvent &event)
    {
        imeChunks = List<String>{};

        if (
            (event.modifiers() & KeyModifier::Control) &&
            event.keyCode() == KeyCode::Key_A
        ) {
            selectAll();
        }
        else if (
            event.scanCode() == ScanCode::Key_Left ||
            event.scanCode() == ScanCode::Key_Right ||
            event.scanCode() == ScanCode::Key_Up ||
            event.scanCode() == ScanCode::Key_Down ||
            event.scanCode() == ScanCode::Key_Home ||
            event.scanCode() == ScanCode::Key_End
        ) {
            int o = textCursor().offset();

            textCursor().step(
                -1 * (event.scanCode() == ScanCode::Key_Left)
                +1 * (event.scanCode() == ScanCode::Key_Right)
                -text().count() * (event.scanCode() == ScanCode::Key_Home)
                +text().count() * (event.scanCode() == ScanCode::Key_End)
            );

            textCursor().lineStep(
                -1 * (event.scanCode() == ScanCode::Key_Up)
                +1 * (event.scanCode() == ScanCode::Key_Down)
            );

            int n = textCursor().offset();

            if (event.modifiers() & KeyModifier::Shift)
            {
                if (selection()) {
                    int s = selection().i0();
                    int e = selection().i1();

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
        }
        else if (event.scanCode() == ScanCode::Key_Backspace)
        {
            Range s = selection();
            if (!s) {
                int i1 = textCursor().offset();
                if (textCursor().step(-1))
                    s = Range{textCursor().offset(), i1};
            }
            if (s) paste(s, String{});
        }
        else if (event.scanCode() == ScanCode::Key_Delete)
        {
            Range s = selection();
            if (!s) {
                int i0 = textCursor().offset();
                if (textCursor().step(1))
                    s = Range{i0, textCursor().offset()};
            }
            if (s) paste(s, String{});
        }
        else if (
            (event.modifiers() & KeyModifier::Control) &&
            event.keyCode() == KeyCode::Key_X
        )
        {
            if (selection()) {
                Application{}.clipboardText(
                    editor_.copy(selection())
                );
                paste(selection(), String{});
            }
        }
        else if (
            (event.modifiers() & KeyModifier::Control) && (
                event.scanCode() == ScanCode::Key_Insert ||
                event.keyCode() == KeyCode::Key_C
            )
        ) {
            if (selection()) {
                Application{}.clipboardText(
                    editor_.copy(selection())
                );
            }
        }
        else if (
            event.scanCode() == ScanCode::Key_Insert || (
                (event.modifiers() & KeyModifier::Control) &&
                event.keyCode() == KeyCode::Key_V
            )
        ) {
            String chunk = Application{}.clipboardText();
            if (chunk.count() > 0) paste(chunk);
        }
        else if (
            (event.modifiers() & KeyModifier::Control) &&
            event.scanCode() == ScanCode::Key_Y
        ) {
            Range range;
            if (event.modifiers() & KeyModifier::Shift) {
                if (editor_.canRedo()) textCursor = TextCursor{};
                range = editor_.redo();
            }
            else {
                if (editor_.canUndo()) textCursor = TextCursor{};
                range = editor_.undo();
            }

            if (range) {
                textCursor = textRun().getTextCursor(range.i1());
                startBlink();
            }
        }
        else if (
            event.scanCode() == ScanCode::Key_LeftShift ||
            event.scanCode() == ScanCode::Key_RightShift
        ) {
            shiftKey_ = true;
        }

        return false;
    });

    onKeyReleased([this](const KeyEvent &event)
    {
        if (
            event.scanCode() == ScanCode::Key_LeftShift ||
            event.scanCode() == ScanCode::Key_RightShift
        )
            shiftKey_ = false;

        return false;
    });
}

String TextInput::State::text() const
{
    return editor_.text();
}

void TextInput::State::text(const String &newValue)
{
    if (editor_.text() != newValue) {
        editor_.paste(Range{0, editor_.text().count()}, newValue);
        textCursor = textRun().getTextCursor(newValue.count());
    }
}

Point TextInput::State::textPos() const
{
    Point p;
    switch(textAlign()) {
        case TextAlign::Center:
            p = Point{std::floor((size()[0] - textRun().size()[0]) / 2), std::ceil(FontMetrics{font()}.ascender())};
            break;
        case TextAlign::Right:
            p = Point{size()[0] - textRun().size()[0], std::ceil(FontMetrics{font()}.ascender())};
            break;
        case TextAlign::Justify:
        case TextAlign::Left:
            p = Point{0, std::ceil(FontMetrics{font()}.ascender())};
            break;
    };
    return p;
}

bool TextInput::State::accept() const
{
    return !accept_ || accept_();
}

Size TextInput::State::preferredSize() const
{
    Size s;

    if (sizerText().count() == 0) {
        s[0] = sp(280);
    }
    else {
        s[0] = TextRun{sizerText(), font()}.size()[0] + sp(1);
    }

    if (textRun().lineCount() < 2) {
        FontMetrics m{font()};
        s[1] += std::ceil(m.ascender()) - std::floor(m.descender());
    }
    else {
        s[1] += textRun().size()[1];
    }

    return s;
}

Size TextInput::State::minSize() const
{
    return Size{0, preferredSize()[1]};
}

Size TextInput::State::maxSize() const
{
    return Size{std::numeric_limits<double>::max(), preferredSize()[1]};
}

void TextInput::State::startBlink()
{
    textCursorVisible = false;
    timer_.startIn(0);
}

void TextInput::State::stopBlink()
{
    timer_.stop();
    textCursorVisible = false;
}

Rect TextInput::State::textInputArea() const
{
    double cx = (textCursor()) ? textCursor().posA()[0] : 0.;

    Point origin { textPos()[0] + cx, 0 };

    return Rect {
        origin,
        Size {
            size()[0] - origin[0],
            size()[1]
        }
    };
}

void TextInput::State::onTextEdited(const String &chunk, int start, int length)
{
    imeChunks = List<String> {
        editor_.copy(Range{0, textCursor().offset()}),
        chunk,
        editor_.copy(Range{textCursor().offset(), text().count()})
    };
}

void TextInput::State::onTextInput(const String &chunk)
{
    Range s = selection();
    if (!s) s = Range{textCursor().offset()};
    paste(s, chunk);
}

void TextInput::State::paste(const String &chunk)
{
    Range s = selection();
    if (!s) s = Range{textCursor().offset()};
    paste(s, chunk);
}

void TextInput::State::paste(Range range, const String &chunk)
{
    if (!(0 <= range.i0() && range.i1() <= text().count())) return;

    String h = chunk;

    String filteredChunk = chunk.replaced("\n", " ");
    if (filter_ && filter_(range, &filteredChunk)) return;

    selection = Range{};
    textCursor = TextCursor{};
    imeChunks = List<String>{};

    Range newRange = editor_.paste(range, filteredChunk);
    if (newRange) {
        textCursor = textRun().getTextCursor(newRange.i1());
        startBlink();
    }
}

void TextInput::State::selectAll()
{
    selection = Range{0, text().count()};
    textCursor().step(text().count());
    startBlink();
}

TextInput::TextInput():
    InputControl{onDemand<State>}
{}

TextInput::TextInput(const String &text):
    InputControl{new State{TextLineEditor{text}}}
{}

TextInput::TextInput(const TextEditor &editor):
    InputControl{new State{editor}}
{}

TextInput::TextInput(CreateState onDemand):
    InputControl{onDemand}
{}

TextInput::TextInput(State *newState):
    InputControl{newState}
{}

TextInput &TextInput::associate(Out<TextInput> self)
{
    return View::associate<TextInput>(self);
}

String TextInput::text() const
{
    return me().text();
}

TextInput &TextInput::text(const String &newValue)
{
    me().text(newValue);
    return *this;
}

TextAlign TextInput::textAlign() const
{
    return me().textAlign();
}

TextInput &TextInput::textAlign(TextAlign newValue)
{
    me().textAlign(newValue);
    return *this;
}

Point TextInput::textPos() const
{
    return me().textPos();
}

Font TextInput::font() const
{
    return me().font();
}

TextInput &TextInput::font(Font newValue)
{
    me().font(newValue);
    return *this;
}

TextInput &TextInput::font(Definition<Font> &&f)
{
    me().font(move(f));
    return *this;
}

String TextInput::sizerText() const
{
    return me().sizerText();
}

TextInput &TextInput::sizerText(const String &newValue)
{
    me().sizerText(newValue);
    return *this;
}

TextInput &TextInput::sizerText(Definition<String> &&f)
{
    me().sizerText(move(f));
    return *this;
}

TextInput &TextInput::accept(Fun<bool()> &&f)
{
    me().accept_ = move(f);
    return *this;
}

TextInput &TextInput::filter(Fun<bool(Range range, InOut<String>)> &&f)
{
    me().filter_ = move(f);
    return *this;
}

void TextInput::selectAll()
{
    me().selectAll();
}

TextEditor TextInput::editor() const
{
    return me().editor_;
}

TextInput::State &TextInput::me()
{
    return View::me().as<State>();
}

const TextInput::State &TextInput::me() const
{
    return View::me().as<State>();
}

} // namespace cc
