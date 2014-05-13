/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Design.h"
#include "WebsiteGenerator.h"

namespace fluxdoc
{

class WebsiteDesign: public Design
{
public:
	static Ref<WebsiteDesign> create() { return new WebsiteDesign; }

private:
	WebsiteDesign()
		: Design("Website")
	{}
};

WebsiteGenerator::WebsiteGenerator()
	: Generator("Website", WebsiteDesign::create())
{}

void WebsiteGenerator::run(Design *design)
{}

} // namespace fluxdoc
