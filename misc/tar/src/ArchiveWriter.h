/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTAR_ARCHIVEWRITER_H
#define FLUXTAR_ARCHIVEWRITER_H

#include <flux/String>

namespace flux {
namespace tar {

class ArchiveWriter: public Object
{
public:
    virtual void writeFile(String path) = 0;
};

}} // namespace flux::tar

#endif // FLUXTAR_ARCHIVEWRITER_H
