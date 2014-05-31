/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_REGISTRY_H
#define FLUXTOKI_REGISTRY_H

#include <flux/SyntaxDefinition.h>
#include <flux/Map.h>
#include "Language.h"
#include "Theme.h"

namespace flux { template<class> class Singleton; }

namespace fluxtoki
{

using namespace flux;

template<class Asset>
class Registration
{
public:
	Registration() {
		static bool firstTime = true;
		if (firstTime) new Asset;
		firstTime = false;
	}
};

class Registry: public Object
{
public:
	bool lookupLanguageByName(String name, Language **language) const;
	bool detectLanguage(String path, String content, Language** language) const;

	int themeCount() const;
	Theme *theme(int i) const;

private:
	friend class Singleton<Registry>;
	friend class Language;

	Registry();

	void registerLanguage(Language *language);
	void registerTheme(Theme *theme);

	typedef Map< String, Ref<Language> > LanguageByName;
	typedef Map< String, Ref<Theme> > ThemeByName;

	Ref<LanguageByName> languageByName_;
	Ref<ThemeByName> themeByName_;
};

Registry *registry();

} // namespace fluxtoki

#endif // FLUXTOKI_REGISTRY_H
