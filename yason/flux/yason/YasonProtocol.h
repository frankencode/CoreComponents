/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXYASON_YASONPROTOCOL_H
#define FLUXYASON_YASONPROTOCOL_H

#include "YasonObject.h"

namespace flux {
namespace yason {

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

	virtual YasonObject *lookup(String className) const {
		YasonObject *prototype = 0;
		if (prototypes_) prototypes_->lookup(className, &prototype);
		return prototype;
	}

	int minCount() const { return minCount_; }
	int maxCount() const { return maxCount_; }
	void minCount(int newCount) { minCount_ = newCount; }
	void maxCount(int newCount) { maxCount_ = newCount; }

	inline bool lookup(const String &className, YasonObject **prototype) const {
		*prototype = lookup(className);
		return *prototype;
	}

protected:
	friend class YasonSyntax;

	YasonProtocol()
		: minCount_(0),
		  maxCount_(flux::intMax)
	{}

	virtual Ref<YasonObject> produce(YasonObject *prototype) const {
		return prototype->produce();
	}

private:
	typedef Map<String, Ref<YasonObject> > Prototypes;
	Prototypes *prototypes() {
		if (!prototypes_) prototypes_ = Prototypes::create();
		return prototypes_;
	}
	Ref<Prototypes> prototypes_;
	int minCount_;
	int maxCount_;
};

}} // namespace flux::yason

#endif // FLUXYASON_YASONPROTOCOL_H
