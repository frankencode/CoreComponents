/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_GENERATOR_H
#define FLUXDOC_GENERATOR_H

#include "Design.h"
#include "Document.h"

namespace flux {
namespace doc {

class Generator: public Object
{
public:
	inline String name() const { return name_; }
	inline Design *prototype() const { return prototype_; }

	virtual void run(Design *design, Document *document) = 0;

protected:
	Generator(String name, Design *prototype);

	String name_;
	Ref<Design> prototype_;
};

}} // namespace flux::doc

#endif // FLUXDOC_GENERATOR_H
