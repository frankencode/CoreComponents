/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/FontInfo>
#include <cc/ui/sdl/FtFontFace>

namespace cc {
namespace ui {

class FtFontProvider;

class FtFontInfo: public FontInfo
{
public:
    static Ref<FtFontInfo> create(int id, String family, FtFaceList *faces) {
        return new FtFontInfo(id, family, faces);
    }

    int id() const { return id_; }
    String family() const { return family_; }

    int faceCount() const { return faces_->count(); }
    FontFace *faceAt(int i) const { return faces_->at(i); }

private:
    friend class FtFontProvider;

    FtFontInfo(int id, String family, FtFaceList *faces):
        id_(id),
        family_(family),
        faces_(faces)
    {}

    inline FtFaceList *faces() const { return faces_; }

    int id_;
    String family_;
    Ref<FtFaceList> faces_;
};

}} // namespace cc::ui

