/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/assert>
#include <cc/Bundle>
#include <cc/DirWalker>
#include <cc/Map>
#include <cc/ui/Application>
#include <cc/ui/sdl/FtFontFace>
#include <cc/ui/sdl/FtFontInfo>
#include <cc/ui/sdl/FtFontMetrics>
#include <cc/ui/sdl/FtFontProvider>

namespace cc {
namespace ui {

FtFontProvider *FtFontProvider::create()
{
    return new FtFontProvider;
}

FtFontProvider::FtFontProvider():
    infoByFamily_(InfoByFamily::create()),
    infoById_(InfoById::create())
{
    addPath(CC_BUNDLE_LOOKUP("ttf"));
    Application::instance()->getDisplayResolution(&xRes_, &yRes_);
}

void FtFontProvider::addPath(String path)
{
    typedef Map<String, Ref<FtFaceList> > FacesByFamily;
    Ref<FacesByFamily> facesByFamily = FacesByFamily::create();
    {
        Ref<DirWalker> walker = DirWalker::open(path);
        walker->setFollowSymlink(true);
        String path; bool isDir;
        while (walker->read(&path, &isDir)) {
            if (isDir) continue;
            if (!path->endsWith(".ttf")) continue;
            Ref<FtFontFace> face = FtFontFace::open(path);
            String family = face->family();
            Ref<FtFaceList> list;
            if (!facesByFamily->lookup(family, &list)) {
                list = FtFaceList::create();
                facesByFamily->insert(face->family(), list);
            }
            list->append(face);
        }
    }

    for (int i = 0; i < facesByFamily->count(); ++i) {
        String family = facesByFamily->keyAt(i);
        Ref<FtFaceList> faces = facesByFamily->valueAt(i);
        int id = infoById_->count();
        Ref<FtFontInfo> info = FtFontInfo::create(id, family, faces);
        infoByFamily_->insert(family, info);
        infoById_->insert(id, info);
    }
}

int FtFontProvider::fontCount() const { return infoByFamily_->count(); }

FontInfo *FtFontProvider::fontInfo(int i) const { return infoByFamily_->valueAt(i); }

Ref<Font> FtFontProvider::selectFont(int i) const
{
    FtFontInfo *info = infoByFamily_->valueAt(i);
    return createFont(info->id_, info->family_);
}

Ref<Font> FtFontProvider::selectFont(String family) const
{
    Ref<FtFontInfo> info;
    if (family == "" || !infoByFamily_->lookup(family, &info)) {
        infoByFamily_->lookup("DejaVu Sans", &info);
        CC_ASSERT(info);
    }
    return createFont(info->id_, info->family_);
}

Ref<FontMetrics> FtFontProvider::getMetrics(Font *font) const
{
    return FtFontMetrics::create(selectFace(font), font->size());
}

FtFontFace *FtFontProvider::selectFace(Font *font) const
{
    Ref<FtFontInfo> info = infoById_->value(font->id());
    FtFaceList *faces = info->faces();
    int targetSelector = faceSelector(font->italic(), font->stretch(), font->weight());
    int bestDelta = 0;
    FtFontFace *bestFace = 0;
    for (int i = 0; i < faces->count(); ++i) {
        FtFontFace *face = faces->at(i);
        int delta =
            targetSelector -
            faceSelector(face->italic(), face->stretch(), face->weight());
        if (delta < 0) delta = -delta;
        if (i == 0 || delta < bestDelta) {
            bestDelta = delta;
            bestFace = face;
        }
    }
    return bestFace;
}

FtFontFace *FtFontProvider::activateFace(Font *font) const
{
    FtFontFace *face = selectFace(font);
    FT_Set_Char_Size(face->face(), 0, font->size() * 64, xRes_, yRes_);
    return face;
}

}} // namespace cc::ui
