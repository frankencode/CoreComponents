/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Variant>

namespace cc {

/** \class Arguments Arguments.h cc/Arguments
  * \brief Command line arguments parser
  */
class Arguments: public Object
{
public:
    /** \brief Read command line arguments and separate options from items
      * \param argc arguments count
      * \param argv argument values
      * \return new Arguments instance
      */
    static Ref<Arguments> parse(int argc, char **argv);

    /** Check validity of command line arguments according to a prototype configuration
      * \param prototype prototype configuration
      * \exception UsageError if a non-existing option or an optino of invalid type is encountered
      */
    void validate(VariantMap *prototype);

    /** Override a configuration object with command line options
      * \param config configuration map
      */
    void override(VariantMap *config) const;

    /// Command line options (e.g.: x=1, -x or --x)
    inline VariantMap *options() const { return options_; }

    /// Command line item arguments
    inline StringList *items() const { return items_; }

    /** Execution path
      * \see Process::exePath()
      */
    inline String execPath() const { return execPath_; }

private:
    Arguments(int argc, char **argv);

    Ref<VariantMap> options_;
    Ref<StringList> items_;
    String execPath_;
};

} // namespace cc
