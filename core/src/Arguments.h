/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_ARGUMENTS_H
#define FLUX_ARGUMENTS_H

#include <flux/Variant>

namespace flux {

class Arguments: public Object
{
public:
    static Ref<Arguments> parse(int argc, char **argv);
    void validate(VariantMap *prototype) const;
    void override(VariantMap *config) const;

    inline VariantMap *options() const { return options_; }
    inline StringList *items() const { return items_; }
    inline String execPath() const { return execPath_; }
    inline String toolName() const { return execPath_->fileName(); }

private:
    Arguments(int argc, char **argv);

    Ref<VariantMap> options_;
    Ref<StringList> items_;
    String execPath_;
};

} // namespace flux

#endif // FLUX_ARGUMENTS_H
