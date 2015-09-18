/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMETA_YASON_H
#define FLUXMETA_YASON_H

#include <flux/syntax/exceptions>
#include <flux/meta/MetaObject>
#include <flux/meta/MetaProtocol>

namespace flux {
namespace yason {

using namespace flux::meta;

Variant parse(const ByteArray *text, const MetaProtocol *protocol = 0);
String stringify(Variant value);

}} // namespace flux::yason

#endif // FLUXMETA_YASON_H
