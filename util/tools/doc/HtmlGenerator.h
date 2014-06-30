/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
