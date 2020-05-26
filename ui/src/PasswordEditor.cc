/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordEditor>
#include <cc/unicode>

namespace cc {
namespace ui {

PasswordEditor::PasswordEditor(const string &bullet):
    bullet_{bullet}
{}

string PasswordEditor::text() const
{
    return text_();
}

string PasswordEditor::password() const
{
    return password_();
}

int PasswordEditor::byteCount() const
{
    return text_()->count();
}

string PasswordEditor::copy(Range range) const
{
    return text_()->copy(range->i0(), range->i1());
}

Range PasswordEditor::paste(Range range, const string &newChunk)
{
    if (!(0 <= range->i0() && range->i1() <= text_()->count()))
        return Range{};

    string mask = string::multiply(bullet_, count(unicode{newChunk}));

    text_ = text_()->paste(range->i0(), range->i1(), mask);

    int m0 = range->i0() / bullet_->count();
    int m1 = range->i1() / bullet_->count();

    auto walker = unicode{newChunk}->begin();
    for (int i = 0; i < m0; ++i) ++walker;
    int p0 = +walker;
    for (int i = m0; i < m1; ++i) ++walker;
    int p1 = +walker;

    password_ = password_()->paste(p0, p1, newChunk);

    return Range { range->i0(), range->i0() + mask->count() };
}

bool PasswordEditor::canUndo() const { return false; }
bool PasswordEditor::canRedo() const { return false; }
Range PasswordEditor::undo() { return Range{}; }
Range PasswordEditor::redo() { return Range{}; }

}} // namespace cc::ui
