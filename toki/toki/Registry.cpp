/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "Registry.h"

namespace fluxtoki
{

Registry::Registry()
	: languageByName_(LanguageByName::create()),
	  themeByName_(ThemeByName::create())
{}

bool Registry::lookupLanguageByName(String name, Language **language) const
{
	return languageByName_->lookup(name, language);
}

int Registry::themeCount() const { return themeByName_->size(); }
Theme *Registry::theme(int i) const { return themeByName_->valueAt(i); }

bool Registry::detectLanguage(String path, String content, Language **language) const
{
	typedef List<Language *> Candidates;
	Ref<Candidates> candidates = Candidates::create();

	for (int i = 0; i < languageByName_->size(); ++i) {
		Language *candidate = languageByName_->valueAt(i);
		if (candidate->pathPattern()->match(path)->valid())
			candidates->append(candidate);
	}

	if (candidates->size() == 1) {
		*language = candidates->at(0);
		return true;
	}

	for (int i = 0; i < candidates->size(); ++i) {
		Language *candidate = candidates->at(i);
		if (candidate->discoverySyntax()) {
			if (candidate->discoverySyntax()->match(content)->valid()) {
				*language = candidate;
				return true;
			}
		}
	}

	return false;
}

void Registry::registerLanguage(Language *language)
{
	languageByName_->insert(language->name(), language);
}

void Registry::registerTheme(Theme *theme)
{
	themeByName_->insert(theme->name(), theme);
}

Registry *registry() { return Singleton<Registry>::instance(); }

} // namespace fluxtoki
