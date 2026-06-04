/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/json>
#include <cc/YasonSyntax>
#include <cc/JsonWriter>
#include <cc/CaptureSink>

namespace cc {

Variant jsonParse(const String &message)
{
    return YasonSyntax{}.parse(message);
}

String jsonStringify(const Variant &value)
{
    CaptureSink sink;
    JsonWriter{sink}.write(value);
    return sink.collect();
}

} // namespace cc
