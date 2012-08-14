/*
 * Property.hpp -- properties and signals
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_PROPERTY_HPP
#define FTL_PROPERTY_HPP

#include "Format.hpp"
#include "Map.hpp"

namespace ftl
{

template<class Value>
class Callback: public Instance {
public:
	virtual void invoke(Value value) = 0;
};

template<class Recipient, class Value>
class Slot: public Callback<Value>
{
public:
	typedef void (Recipient::* Method)(Value);

	Slot()
		: recipient_(0),
		  method_(0)
	{}

	Slot(Ref<Recipient> recipient, Method method)
		: recipient_(recipient),
		  method_(method)
	{}

	void invoke(Value value) { (recipient_->*method_)(value); }

private:
	Ref<Recipient> recipient_;
	Method method_;
};

template<class Value>
class Signal: public Instance
{
public:
	void emit(Value value) {
		for (int i = 0, n = callbacks_.length(); i < n; ++i) {
			Ref<CallbackList> list = callbacks_.at(i).value();
			for (int j = 0, m = list->length(); j < m; ++j)
				list->at(j)->invoke(value);
		}
	}

	template<class Recipient>
	void connect(Recipient* recipient, void (Recipient::* method)(Value)) {
		Ref<CallbackList, Owner> list;
		if (!callbacks_.lookup(recipient, &list)) {
			list = CallbackList::newInstance();
			callbacks_.insert(recipient, list);
		}
		list->append(new Slot<Recipient, Value>(recipient, method));
	}

	void disconnect(void* recipient) {
		callbacks_.remove(recipient);
	}

private:
	friend class Connection;
	typedef List< Ref<Callback<Value>, Owner> > CallbackList;
	Map<void*, Ref<CallbackList, Owner> > callbacks_;
};

template<class T>
class Property
{
public:
	Property() {}
	Property(const T& b): value_(b) {}

	inline operator const T&() const { return value_; }
	inline Property& operator=(const T& b) {
		if (b != value_) {
			value_ = b;
			valueChanged_->emit(value_);
		}
		return *this;
	}

	inline Ref< Signal<T> > valueChanged() const { return valueChanged_; }

	inline String toString() const { return Format() << value_; }

private:
	Ref<Signal<T>, OnDemand> valueChanged_;
	T value_;
};

} // namespace ftl

#endif // FTL_PROPERTY_HPP
