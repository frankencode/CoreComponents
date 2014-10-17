/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_REGISTRY_H
#define FLUXDOC_REGISTRY_H

#include <flux/MetaProtocol>

namespace flux {

template<class> class Singleton;

namespace doc {

class Generator;

class Registry: public Object
{
public:
    Generator* generatorByName(String name) const;
    Generator* generatorByIndex(int index) const;
    int generatorCount() const;

    inline MetaProtocol *designProtocol() const { return designProtocol_; }

private:
    friend class Singleton<Registry>;
    friend class Generator;

    Registry();
    void registerGenerator(Generator *generator);

    typedef Map< String, Ref<Generator> > GeneratorByName;
    Ref<GeneratorByName> generatorByName_;

    Ref<MetaProtocol> designProtocol_;
};

Registry *registry();

}} // namespace flux::doc

#endif // FLUXDOC_REGISTRY_H
