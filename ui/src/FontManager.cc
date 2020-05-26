/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/Map>
#include <cc/ValueSource>
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

void FontManager::addPath(const string &dirPath, const string &namePrefix)
{
    Ref<DirWalker> walker = DirWalker::open(dirPath);
    string path;
    bool isDir = false;
    while (walker->read(&path, &isDir)) {
        if (!isDir && isFontFace(path)) {
            if (namePrefix != "" && !path->fileName()->startsWith(namePrefix))
                continue;
            addFontFace(openFontFace(path));
        }
    }
}

Ref< Source<const FontFamily *> > FontManager::getFontFamilies() const
{
    return ValueSource<FontFamilies, const FontFamily *>::open(fontFamilies_);
}

const FontFamily *FontManager::selectFontFamily(const string &family) const
{
    string key = family->normalize();
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
    fontFamilies_{FontFamilies::create()}
{}

FontManager::~FontManager()
{}

bool FontManager::isFontFace(const string &path) const
{
    string suffix = path->fileSuffix();
    mutate(suffix)->downcaseInsitu();
    return suffix == "ttf" || suffix == "woff" || suffix == "otf" || suffix == "ttc";
}

void FontManager::addFontFace(const FontFace *fontFace)
{
    Ref<FontFamily> fontFamily;
    string key = fontFace->family()->normalize();
    if (!fontFamilies_->lookup(key, &fontFamily)) {
        fontFamily = FontFamily::create(fontFace->family(), fontFace->pitch());
        fontFamilies_->insert(key, fontFamily);
    }
    fontFamily->fontFaces_->append(fontFace);
}

const FontFamily *FontManager::selectNearestFontFamily(const string &name) const
{
    Ref<StringList> searchPattern = familySearchPattern(name);

    const FontFamily *bestMatch = 0;
    int bestMatchRank = 0;

    for (const FontFamily *candidate: getFontFamilies()) {
        Ref<StringList> candidatePattern = familySearchPattern(candidate->name());
        int matchRank = 0;
        for (string component: searchPattern) {
            if (candidatePattern->contains(component)) {
                matchRank += 2;
            }
            else {
                for (string candidateComponent: candidatePattern) {
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

Ref<StringList> FontManager::familySearchPattern(const string &name)
{
    Ref<StringList> searchPattern = StringList::create();
    for (string component: name->toLower()->split(' ')) {
        if (component != string{}) searchPattern->append(component);
    }
    return searchPattern;
}

}} // namespace cc::ui
