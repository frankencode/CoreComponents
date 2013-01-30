 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_PROPERTY_HPP
#define FTL_PROPERTY_HPP

#include "format.hpp"
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

	Slot(Recipient *recipient, Method method)
		: recipient_(recipient),
		  method_(method)
	{}

	void invoke(Value value) { (recipient_->*method_)(value); }

private:
	Recipient *recipient_;
	Method method_;
};

template<class Value>
class Signal: public Instance
{
public:
	inline static hook<Signal> create() { return new Signal; }

	void emit(Value value) {
		for (int i = 0; i < callbacks()->length(); ++i) {
			CallbackList *list = callbacks()->valueAt(i);
			for (int j = 0; j < list->length(); ++j)
				list->at(j)->invoke(value);
		}
	}

	template<class Recipient>
	void connect(Recipient *recipient, void (Recipient::* method)(Value)) {
		hook<CallbackList> list;
		if (!callbacks()->lookup(recipient, &list)) {
			list = CallbackList::create();
			callbacks()->insert(recipient, list);
		}
		list->append(new Slot<Recipient, Value>(recipient, method));
	}

	void disconnect(void *recipient) {
		callbacks()->remove(recipient);
	}

private:
	friend class Connection;
	typedef List< hook< Callback<Value> > > CallbackList;
	typedef Map<void*, hook<CallbackList> > CallbackListByRecipient;

	Signal() {}

	inline CallbackListByRecipient *callbacks() {
		if (!callbacks_) callbacks_ = CallbackListByRecipient::create();
		return callbacks_;
	}

	hook<CallbackListByRecipient> callbacks_;
};

template<class T>
class property
{
public:
	property() {}
	property(const T &b): value_(b) {}

	inline operator const T&() const { return value_; }
	inline property &operator=(const T &b) {
		if (b != value_) {
			value_ = b;
			if (valueChanged_)
				valueChanged_->emit(value_);
		}
		return *this;
	}

	Signal<T> *valueChanged() {
		if (!valueChanged_) valueChanged_ = Signal<T>::create();
		return valueChanged_;
	}

	inline operator string() const { return format() << value_; }

	property *operator->() { return this; }
	const property *operator->() const { return this; }

private:
	hook< Signal<T> > valueChanged_;
	T value_;
};

} // namespace ftl

#endif // FTL_PROPERTY_HPP
