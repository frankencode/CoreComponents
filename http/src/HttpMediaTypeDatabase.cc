/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpMediaTypeDatabase>
#include <cc/Casefree>
#include <cc/Prefix>
#include <cc/Map>

namespace cc {

struct HttpMediaTypeDatabase::State: public Object::State
{
    State()
    {
        mediaTypeByFileSuffix_.insert("xhtml", "application/xhtml+xml");
        mediaTypeByFileSuffix_.insert("css", "text/css");
        mediaTypeByFileSuffix_.insert("js", "text/javascript");
        mediaTypeByFileSuffix_.insert("svg", "image/svg+xml");
        mediaTypeByFileSuffix_.insert("webp", "image/webp");

        mediaTypeByContentPrefix_.insert("<!DOCTYPE html", "text/html");
        mediaTypeByContentPrefix_.insert("<html", "text/html");
        mediaTypeByContentPrefix_.insert("<?xml", "application/xml");
        mediaTypeByContentPrefix_.insert("<svg", "image/svg+xml");
        mediaTypeByContentPrefix_.insert("\377\330", "image/jpeg");
        mediaTypeByContentPrefix_.insert("\211PNG", "image/png");
        mediaTypeByContentPrefix_.insert("II*\0", "image/tiff");
        mediaTypeByContentPrefix_.insert("MM*\0", "image/tiff");
        mediaTypeByContentPrefix_.insert("GIF87a", "image/gif");
        mediaTypeByContentPrefix_.insert("GIF89a", "image/gif");
    }

    String lookup(const String &path, const String &content) const
    {
        String value;

        if (path != "") {
            String suffix = path.fileSuffix();
            if (suffix != "") {
                if (mediaTypeByFileSuffix_.lookup(suffix, &value))
                    return value;
            }
        }
        if (mediaTypeByContentPrefix_.lookupPattern(Prefix{content}, &value)) {
            return value;
        }

        return value;
    }

    Map<Casefree<String>, String> mediaTypeByFileSuffix_;
    Map<String, String> mediaTypeByContentPrefix_;
};

HttpMediaTypeDatabase::HttpMediaTypeDatabase()
{
    initOnce<State>();
}

String HttpMediaTypeDatabase::lookup(const String &path, const String &content) const
{
    return me().lookup(path, content);
}

const HttpMediaTypeDatabase::State &HttpMediaTypeDatabase::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
