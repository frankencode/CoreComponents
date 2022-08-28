/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CsvSource>
#include <cc/CsvSyntax>

namespace cc {

struct CsvSource::State final: public Object::State
{
    explicit State(const String &text):
        text_{text}
    {
        text_.trimBom();
    }

    bool read(Out<List<String>> record)
    {
        record = CsvSyntax{}.parse(text_, &offset_);
        return record->count() > 0;
    }

    String text_;
    long offset_ { 0 };
};

CsvSource::CsvSource(const String &text):
    Object{new State{text}}
{}

bool CsvSource::read(Out<List<String>> record)
{
    return me().read(record);
}

CsvSource::State &CsvSource::me()
{
    return Object::me.as<State>();
}

} // namespace cc
