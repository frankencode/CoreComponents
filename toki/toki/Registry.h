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

#include <flux/Singleton.h>
#include <flux/SyntaxDefinition.h>
#include "Language.h"
#include "Theme.h"

namespace fluxtoki
{

using namespace flux;

class Registry: public Object, public Singleton<Registry>
{
public:
	bool lookupLanguageByName(String name, Language **language) const;
	bool detectLanguage(String path, String content, Language** language) const;

	int themeCount() const;
	Theme *theme(int i) const;

private:
	friend class Singleton<Registry>;

	Registry();

	void registerLanguage(Language *language);
	void registerTheme(Theme *theme);

	typedef Map< String, Ref<Language> > LanguageByName;
	typedef Map< String, Ref<Theme> > ThemeByName;

	Ref<LanguageByName> languageByName_;
	Ref<ThemeByName> themeByName_;
};

inline Registry *registry() { return Registry::instance(); }

} // namespace fluxtoki

#endif // FLUXTOKI_REGISTRY_H
