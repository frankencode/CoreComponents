/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FontManager>
#include <cc/PlatformManager>
#include <cc/DirWalk>
#include <cc/input>

namespace cc {

Font FontManager::State::fixup(Font font)
{
    Font f = font;
    if (f.family() == "") f.family(style().defaultFont().family());
    if (f.size() <= 0) f.size(style().defaultFont().size());
    if (f.smoothing() == FontSmoothing::Default)
        f.smoothing(app().fontSmoothing());
    return f;
}

List<String> FontManager::State::familySearchPattern(const String &name)
{
    List<String> searchPattern;
    for (String component: name.downcased().split(' ')) {
        if (component != "") searchPattern.append(component);
    }
    return searchPattern;
}

bool FontManager::State::isFontFace(const String &path) const
{
    String suffix = path.fileSuffix();
    suffix.downcase();
    return suffix == "ttf" || suffix == "woff2" || suffix == "woff" || suffix == "otf" || suffix == "ttc";
}

FontFamily FontManager::State::selectFontFamily(const String &name) const
{
    String key = normalize(name);
    FontFamily fontFamily;
    if (fontFamilies_.lookup(key, &fontFamily))
        return fontFamily;

    return selectNearestFontFamily(name);
}

FontFamily FontManager::State::selectNearestFontFamily(const String &name) const
{
    List<String> searchPattern = familySearchPattern(name);

    FontFamily bestMatch;
    int bestMatchRank = 0;

    for (const FontFamily &candidate: fontFamilies_) {
        List<String> candidatePattern = familySearchPattern(candidate.name());
        int matchRank = 0;
        for (const String &component: searchPattern) {
            if (candidatePattern.find(component)) {
                matchRank += 2;
            }
            else {
                for (const String &candidateComponent: candidatePattern) {
                    if (candidateComponent.find(component)) {
                        ++matchRank;
                        break;
                    }
                }
            }
        }
        if (matchRank >= bestMatchRank) {
            if (matchRank == bestMatchRank && bestMatch) {
                if (bestMatch.name().count() < candidate.name().count())
                    continue;
            }
            bestMatch = candidate;
            bestMatchRank = matchRank;
        }
    }

    return bestMatch;
}

FontManager::FontManager()
{
    *this = PlatformManager{}.activePlugin().fontManager();
}

void FontManager::addPath(const String &dirPath, const String &namePrefix)
{
    for (const String &path: DirWalk{dirPath, DirWalk::FilesOnly})
    {
        if (!me().isFontFace(path)) continue;
        if (namePrefix != "" && !path.fileName().startsWith(namePrefix)) continue;
        if (path.endsWith("ttc")) continue;
        addFontFace(me().openFontFace(path));
    }
}

void FontManager::addFontFace(const FontFace &fontFace)
{
    FontFamily fontFamily;
    if (!me().fontFamilies_.lookup(fontFace.family(), &fontFamily)) {
        fontFamily = FontFamily{fontFace.family(), fontFace.pitch()};
        me().fontFamilies_.insert(fontFamily);
    }
    fontFamily.me().fontFaces_.append(fontFace);
}

} // namespace cc
