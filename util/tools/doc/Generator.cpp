/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Registry.h"
#include "Generator.h"

namespace flux {
namespace doc {

Generator::Generator(String name, Design *prototype)
    : name_(name),
      prototype_(prototype)
{
    registry()->registerGenerator(this);
}

}} // namespace flux::doc
