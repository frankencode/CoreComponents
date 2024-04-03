/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FtFontFace>
#include <cc/input>

namespace cc {

class FtFontError: public FontError
{
public:
    FtFontError(const String &path, FT_Error error):
        path_{path},
        error_{error}
    {}

    String message() const override
    {
        return Format{"Failed to load font face \"%%\": error = %%\n"} << path_ << error_;
    }

private:
    String path_;
    FT_Error error_;
};

FtFontFace::State::State(const String &path):
    FontFace::State{path}
{
    FT_Error error = FT_New_Face(ftLibrary_.ftLibrary(), path_, 0, &face_);
    if (error) throw FtFontError(path_, error);

    family_ = normalize(face_->family_name);

    TT_OS2 *os2 = (TT_OS2 *)FT_Get_Sfnt_Table(face_, ft_sfnt_os2);
    if (os2) weight_ = static_cast<Weight>(os2->usWeightClass / 100);
    else weight_ = (face_->style_flags & FT_STYLE_FLAG_BOLD) ? Weight::Bold : Weight::Normal;
    if (os2) stretch_ = static_cast<Stretch>(os2->usWidthClass);
    else stretch_ = Stretch::Normal;

    slant_ = (face_->style_flags & FT_STYLE_FLAG_ITALIC) ? Slant::Italic : Slant::Normal;
    pitch_ = (face_->face_flags & FT_FACE_FLAG_FIXED_WIDTH) ? Pitch::Fixed : Pitch::Variable;
}

FtFontFace::State::~State()
{
    if (face_) FT_Done_Face(face_);
}

FtFontFace::FtFontFace(const String &path):
    FontFace{new State{path}}
{}

FT_Face FtFontFace::ftFace() const
{
    return me().face_;
}

const FtFontFace::State &FtFontFace::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
