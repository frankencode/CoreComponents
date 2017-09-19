/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/ui/FontProvider>

namespace cc {
namespace ui {

Ref<Font> FontProvider::selectFont(String family) const
{
    String target = family->downcase()->simplify();
    Ref<StringList> targetBits = target->split(' ');
    int targetSize = 0;
    for (int j = 0; j < targetBits->count(); ++j)
        targetSize += targetBits->at(j)->count();

    int bestIndex = 0;
    int bestMatch = -1;

    for (int i = 0, n = fontCount(); i < n; ++i) {
        FontInfo *info = fontInfo(i);
        String candidate = info->family()->downcase()->simplify();
        if (candidate == target) {
            bestIndex = i;
            break;
        }
        Ref<StringList> candidateBits = candidate->split(' ');
        int matchCount = candidateBits->count();
        for (int j = 0; j < targetBits->count(); ++j) {
            String bit = targetBits->at(j);
            if (candidate->contains(bit))
                --matchCount;
        }
        if (matchCount == candidateBits->count()) continue;
        if (bestMatch == -1 || matchCount < bestMatch) {
            bestIndex = i;
            if (matchCount == 0) break;
            bestMatch = matchCount;
        }
    }

    return selectFont(bestIndex);
}

int FontProvider::faceSelector(bool italic, Font::Stretch stretch, Font::Weight weight)
{
    return ((italic * 10000) + stretch) + weight;
}

Ref<Font> FontProvider::createFont(int id, String family)
{
    return new Font(id, family);
}

}} // namespace cc::ui
