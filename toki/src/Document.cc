/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/File>
#include <cc/toki/Document>

namespace cc {
namespace toki {

Ref<Document> Document::load(const String &path)
{
    return Document::create(File{path}->map(), path);
}

void Document::save()
{
    File::save(path_, text_);
}

Document::Document(const String &text, const String &path):
    path_{path},
    text_{text},
    spans_{Spans::create()}
{}

}} // namespace cc::toki
