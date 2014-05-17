/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_YASONPROTOCOL_H
#define FLUX_YASONPROTOCOL_H

#include "YasonObject.h"

namespace flux
{

class YasonProtocol: public Object
{
public:
	inline static Ref<YasonProtocol> create() {
		return new YasonProtocol;
	}

	template<class Prototype>
	Prototype *define(const String &className) {
		Ref<Prototype> prototype = Prototype::create(className);
		define(prototype);
		return prototype;
	}

	template<class Prototype>
	Prototype *define() {
		Ref<Prototype> prototype = Prototype::create();
		define(prototype);
		return prototype;
	}

	YasonObject *define(YasonObject *prototype) {
		prototype->define();
		prototypes()->insert(prototype->className(), prototype);
		return prototype;
	}

	template<class Prototype>
	static Ref<YasonObject> createPrototype() {
		Ref<YasonObject> prototype = Prototype::create();
		prototype->define();
		return prototype;
	}

	virtual YasonObject *lookup(String className) {
		YasonObject *prototype = 0;
		prototypes()->lookup(className, &prototype);
		return prototype;
	}

	virtual int maxCount() const { return flux::intMax; }

	inline bool lookup(const String &className, YasonObject **prototype) {
		*prototype = lookup(className);
		return *prototype;
	}

protected:
	friend class YasonSyntax;

	virtual Ref<YasonObject> produce(YasonObject *prototype) {
		return prototype->produce();
	}

private:
	typedef Map<String, Ref<YasonObject> > Prototypes;
	Prototypes *prototypes() {
		if (!prototypes_) prototypes_ = Prototypes::create();
		return prototypes_;
	}
	Ref<Prototypes> prototypes_;
};

} // namespace flux

#endif // FLUX_YASONPROTOCOL_H
