/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>

using namespace cc;

template<class T>
class StoragePolicy {
public:
    typedef T StorageType;
};

template<class T>
class StoragePolicy<T*> {
public:
    typedef Ref<T> StorageType;
};

template<class T>
class TestList: public List< typename StoragePolicy<T>::StorageType > {
public:
    TestList() {}
};

class Bird: public Object {
public:
    Bird(int id): id_(id) { fout("Bird(): id_ = %%\n") << id_; }
    ~Bird() { fout("~Bird(): id_ = %%\n") << id_; }
    int id() const { return id_; }
private:
    int id_;
};

typedef TestList<Bird*> Birds;

template<class T>
Ref<T> fly(T *x) { return x; }

int main(int argc, char **argv)
{
    fly(new Birds)
        << new Bird(1)
        << new Bird(2);
    return 0;
}
