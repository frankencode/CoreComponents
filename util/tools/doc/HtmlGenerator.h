/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_HTMLGENERATOR_H
#define FLUXDOC_HTMLGENERATOR_H

#include <flux/Singleton.h>
#include "Generator.h"

namespace flux {
namespace doc {

class HtmlGeneratorRegistration;

class HtmlWriter;

class HtmlGenerator: public Generator
{
public:
	virtual void run(Design *design, Document *document);

private:
	friend class HtmlGeneratorRegistration;
	HtmlGenerator();

	template<class Writer>
	void define() {
		writerMap_->insert(Writer::className(), Writer::create());
	}

	typedef Map< String, Ref<HtmlWriter> > WriterMap;
	Ref<WriterMap> writerMap_;
};

class HtmlGeneratorRegistration: public Object
{
public:
	HtmlGeneratorRegistration() {
		if (firstTime())
			new HtmlGenerator;
	}
private:
	static bool firstTime() { static int count = 0; return count++ == 0; }
};

namespace { HtmlGeneratorRegistration registration; }

}} // namespace flux::doc

#endif // FLUXDOC_HTMLGENERATOR_H
