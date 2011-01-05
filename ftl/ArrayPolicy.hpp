/*
 * ArrayPolicy.hpp -- array policies
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_ARRAYPOLICY_HPP
#define FTL_ARRAYPOLICY_HPP

namespace ftl
{

template<class T>
class EmptyArray {
public:
	inline static T* isEmpty() { return 0; }
};

template<>
class EmptyArray<char> {
public:
	inline static char* isEmpty() { return const_cast<char*>(""); }
};

template<>
class EmptyArray<const char> {
public:
	inline static const char* isEmpty() { return ""; }
};

template<class T>
class DeepCopyArray {
public:
	inline static T* isEmpty() {
		return EmptyArray<T>::isEmpty();
	}
	inline static void initEmpty(T*& data_, int& size_) {
		size_ = 0;
		data_ = isEmpty();
	}
	inline static void alloc(T*& data_, int& size_, int size) {
		if (size > 0) {
			size_ = size;
			data_ = new T[size];
		}
		else {
			size_ = 0;
			data_ = isEmpty();
		}
	}
	inline static void free(T*& data_, int& size_) {
		if (size_ > 0) {
			delete[] data_;
			size_ = 0;
			data_ = isEmpty();
		}
	}
	inline static void assign(T*& data_, int& size_, const T* data, int size) {
		if (size_ != size) {
			free(data_, size_);
			alloc(data_, size_, size);
		}
		for (int i = 0; i < size; ++i) data_[i] = data[i];
	}
	inline static void clear(T*& data_, int& size_, T zero) {
		for (int i = 0; i < size_; ++i) data_[i] = zero;
	}
};

template<class T>
class DeepCopyZeroTerminatedArray {
public:
	inline static T* isEmpty() {
		return EmptyArray<T>::isEmpty();
	}
	inline static void initEmpty(T*& data_, int& size_) {
		size_ = 0;
		data_ = isEmpty();
	}
	inline static void alloc(T*& data_, int& size_, int size) {
		if (size > 0) {
			size_ = size;
			data_ = new T[size + 1];
			data_[size] = 0;
		}
		else {
			size_ = 0;
			data_ = isEmpty();
		}
	}
	inline static void free(T*& data_, int& size_) {
		if (size_ > 0) {
			delete[] data_;
			size_ = 0;
			data_ = isEmpty();
		}
	}
	inline static void assign(T*& data_, int& size_, const T* data, int size) {
		if (size_ != size) {
			free(data_, size_);
			alloc(data_, size_, size);
		}
		for (int i = 0; i < size; ++i) data_[i] = data[i];
	}
	inline static void clear(T*& data_, int& size_, T zero) {
		for (int i = 0; i < size_; ++i) data_[i] = zero;
	}
};

template<class T>
class ShallowCopyArray {
public:
	inline static T* isEmpty() {
		return EmptyArray<T>::isEmpty();
	}
	inline static void initEmpty(T*& data_, int& size_) {
		size_ = 0;
		data_ = isEmpty();
	}
	inline static void free(T*& data_, int& size_) {
		size_ = 0;
		data_ = isEmpty();
	}
	inline static void assign(T*& data_, int& size_, const T* data, int size) {
		size_ = size;
		data_ = const_cast<T*>(data);
	}
};

template<class T>
class ArrayPolicy: public DeepCopyArray<T> {};

template<class T>
class ArrayPolicy<const T>: public ShallowCopyArray<const T> {};

} // namespace ftl

#endif // FTL_ARRAYPOLICY_HPP
