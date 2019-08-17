/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/MediaTypeDatabase>
#include <cc/Singleton>

namespace cc {
namespace node {

const MediaTypeDatabase *MediaTypeDatabase::instance()
{
    return Singleton<MediaTypeDatabase>::instance();
}

MediaTypeDatabase::MediaTypeDatabase():
    mediaTypeByPathSuffix_{PrefixTree<char, String>::create()},
    mediaTypeByContentPrefix_{PrefixTree<char, String>::create()}
{
    mediaTypeByPathSuffix_->insert("xhtml", "application/xhtml+xml");
    mediaTypeByPathSuffix_->insert("css", "text/css");
    mediaTypeByPathSuffix_->insert("js", "text/javascript");
    mediaTypeByPathSuffix_->insert("svg", "image/svg+xml");
    mediaTypeByPathSuffix_->insert("webp", "image/webp");
    mediaTypeByContentPrefix_->insert("<!DOCTYPE html", "text/html");
    mediaTypeByContentPrefix_->insert("<html", "text/html");
    mediaTypeByContentPrefix_->insert("<?xml", "application/xml");
    mediaTypeByContentPrefix_->insert("<svg", "image/svg+xml");
    mediaTypeByContentPrefix_->insert("\377\330", "image/jpeg");
    mediaTypeByContentPrefix_->insert("\211PNG", "image/png");
    mediaTypeByContentPrefix_->insert("II*\0", "image/tiff");
    mediaTypeByContentPrefix_->insert("MM*\0", "image/tiff");
    mediaTypeByContentPrefix_->insert("GIF87a", "image/gif");
    mediaTypeByContentPrefix_->insert("GIF89a", "image/gif");
}

String MediaTypeDatabase::lookup(const String &path, const String &content) const
{
    String value;
    if (path != "") {
        String suffix = path->fileSuffix();
        if (suffix != "") {
            if (mediaTypeByPathSuffix_->lookup(suffix, &value, false))
                return value;
        }
    }
    mediaTypeByContentPrefix_->lookup(content, &value, false);
    return value;
}

}} // namespace cc::node
