/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "WebsiteGenerator.h"

namespace fluxdoc
{

class WebsiteSettings: public YasonObject
{
public:
	static Ref<WebsiteSettings> create() { return new WebsiteSettings; }
private:
	WebsiteSettings()
		: YasonObject("Website")
	{
		// TODO
	}
};

WebsiteGenerator::WebsiteGenerator()
	: Generator("Website", WebsiteSettings::create())
{}

void WebsiteGenerator::run()
{}

} // namespace fluxdoc
