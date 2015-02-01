/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/String>
#include <flux/tar/ArchiveWriter>

namespace fluxtar {

using namespace flux;
using namespace flux::tar;

void pack(String path, ArchiveWriter *archive, bool verbose);

} // namespace fluxtar
