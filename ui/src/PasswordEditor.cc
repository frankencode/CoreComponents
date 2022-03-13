/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordEditor>
#include <cc/Unicode>

namespace cc::ui {

PasswordEditor::State::State(const String &bullet):
    bullet_{bullet}
{}

long PasswordEditor::State::count() const
{
    return text_().count();
}

Range PasswordEditor::State::textToPasswordRange(Range range) const
{
    long m0 = range.i0() / bullet_.count();
    long m1 = range.i1() / bullet_.count();

    auto walker = Unicode{password_()}.begin();
    for (long i = 0; i < m0; ++i) ++walker;
    long p0 = +walker;
    for (long i = m0; i < m1; ++i) ++walker;
    long p1 = +walker;

    return Range{p0, p1};
}

String PasswordEditor::State::copy(Range range) const
{
    Range passwordRange = textToPasswordRange(range);
    return password_().copy(passwordRange.i0(), passwordRange.i1());
}

String PasswordEditor::State::text() const
{
    return text_();
}

Range PasswordEditor::State::paste(Range range, const String &chunk)
{
    if (!(0 <= range.i0() && range.i1() <= text_().count()))
        return Range{};

    String mask = bullet_ * Unicode{chunk}.count();

    text_ = text_().paste(range.i0(), range.i1(), mask);

    Range passwordRange = textToPasswordRange(range);

    password_ = password_().paste(passwordRange.i0(), passwordRange.i1(), chunk);

    return Range{range.i0(), range.i0() + mask.count()};
}

bool PasswordEditor::State::canUndo() const
{
    return false;
}

bool PasswordEditor::State::canRedo() const
{
    return false;
}

Range PasswordEditor::State::undo()
{
    return Range{};
}

Range PasswordEditor::State::redo()
{
    return Range{};
}

PasswordEditor::PasswordEditor(const String &bullet):
    TextEditor{new State{bullet}}
{}

String PasswordEditor::password() const
{
    return me().password_;
}

PasswordEditor::State &PasswordEditor::me()
{
    return Object::me.as<State>();
}

const PasswordEditor::State &PasswordEditor::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::ui
