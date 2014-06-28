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

namespace flux {

template<class> class Singleton;

namespace toki {

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
	bool detectLanguage(String path, String text, Language **language) const;

	inline int languageCount() const { return languageByName_->count(); }
	inline const Language *languageAt(int i) const { return languageByName_->valueAt(i); }

private:
	friend class Singleton<Registry>;
	friend class Language;

	Registry();

	void registerLanguage(Language *language);

	typedef Map< String, Ref<Language> > LanguageByName;
	Ref<LanguageByName> languageByName_;
};

Registry *registry();

}} // namespace flux::toki

#endif // FLUXTOKI_REGISTRY_H
