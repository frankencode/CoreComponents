/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Variant>
#include <flux/Map>

namespace flux {

template class Map<String, String>;
template class Map<String, Variant>;

} // namespace flux
