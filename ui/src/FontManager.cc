/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/Map>
#include <cc/DirWalker>
#include <cc/ui/PlatformPlugin>
#include <cc/ui/StylePlugin>
#include <cc/ui/Application>
#include <cc/ui/FontManager>

namespace cc {
namespace ui {

FontManager *FontManager::instance()
{
    return PlatformPlugin::instance()->fontManager();
}

void FontManager::addPath(const String &dirPath)
{
    Ref<DirWalker> walker = DirWalker::open(dirPath);
    String path;
    bool isDir = false;
    while (walker->read(&path, &isDir)) {
        if (!isDir && isFontFace(path))
            addFontFace(openFontFace(path));
    }
}

Ref< Source<const FontFamily *> > FontManager::getFontFamilies() const
{
    return fontFamilies_->getAllValues<const FontFamily *>();
}

const FontFamily *FontManager::selectFontFamily(const String &family) const
{
    String key = family->normalize();
    Ref<FontFamily> fontFamily;
    if (fontFamilies_->lookup(key, &fontFamily))
        return fontFamily;

    return selectNearestFontFamily(family);
}

Font FontManager::fixup(const Font &font)
{
    Font f = font;
    if (!f->family_) f->family_ = StylePlugin::instance()->defaultFont()->family();
    if (f->size_ <= 0) f->size_ = StylePlugin::instance()->defaultFont()->size();
    if (f->smoothing() == FontSmoothing::Default)
        f->smoothing_ = static_cast<uint8_t>( Application::instance()->fontSmoothing() );
    return f;
}

FontManager::FontManager():
    fontFamilies_(FontFamilies::create())
{}

FontManager::~FontManager()
{}

bool FontManager::isFontFace(const String &path) const
{
    String suffix = path->fileSuffix();
    mutate(suffix)->downcaseInsitu();
    return suffix == "otf" || suffix == "ttf" || suffix == "ttc";
}

void FontManager::addFontFace(const FontFace *fontFace)
{
    Ref<FontFamily> fontFamily;
    String key = fontFace->family()->normalize();
    if (!fontFamilies_->lookup(key, &fontFamily)) {
        fontFamily = FontFamily::create(fontFace->family(), fontFace->pitch());
        fontFamilies_->insert(key, fontFamily);
    }
    fontFamily->fontFaces_->append(fontFace);
}

const FontFamily *FontManager::selectNearestFontFamily(const String &name) const
{
    Ref<StringList> searchPattern = familySearchPattern(name);

    const FontFamily *bestMatch = 0;
    int bestMatchRank = 0;

    for (const FontFamily *candidate: getFontFamilies()) {
        Ref<StringList> candidatePattern = familySearchPattern(candidate->name());
        int matchRank = 0;
        for (String component: searchPattern) {
            if (candidatePattern->contains(component)) {
                matchRank += 2;
            }
            else {
                for (String candidateComponent: candidatePattern) {
                    if (candidateComponent->contains(component)) {
                        ++matchRank;
                        break;
                    }
                }
            }
        }
        if (matchRank >= bestMatchRank) {
            if (matchRank == bestMatchRank && bestMatch) {
                if (bestMatch->name()->count() < candidate->name()->count())
                    continue;
            }
            bestMatch = candidate;
            bestMatchRank = matchRank;
        }
    }

    return bestMatch;
}

Ref<StringList> FontManager::familySearchPattern(const String &name)
{
    Ref<StringList> searchPattern = StringList::create();
    for (String component: name->toLower()->split(' ')) {
        if (component != String{}) searchPattern->append(component);
    }
    return searchPattern;
}

}} // namespace cc::ui
