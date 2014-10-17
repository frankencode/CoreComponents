/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/Arguments>
#include <flux/ResourceGuard>
#include "Document.h"
#include "Design.h"
#include "Registry.h"
#include "Generator.h"

using namespace flux;
using namespace flux::doc;

int main(int argc, char **argv)
{
    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);
        Ref<VariantMap> options = arguments->options();
        Ref<StringList> items = arguments->items();

        String defaultSourcePath = str(options->value("source"));
        Ref<Document> defaultDocument;
        if (defaultSourcePath != "") defaultDocument = Document::load(defaultSourcePath);

        for (int i = 0; i < items->count(); ++i) {
            String designPath = items->at(i);
            Ref<Design> design;
            {
                ResourceGuard guard(designPath);
                design = yason::parse(File::open(designPath)->map(), registry()->designProtocol());
            }
            arguments->validate(design);
            arguments->override(design);

            Ref<Document> document = defaultDocument;
            String sourcePath = design->value("source", "");
            if (defaultSourcePath != sourcePath && sourcePath != "") {
                document = Document::load(sourcePath);
            }
            if (!document) document = Document::parse("");

            Generator *generator = registry()->generatorByName(design->className());
            generator->run(design, document);
        }
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [OPTION]... [FILE]...\n"
            "Produce documentation using one or more design FILEs.\n"
            "\n"
            "Options:\n"
            "  -source  source file\n"
        ) << String(argv[0])->fileName();
        return 1;
    }
    catch (Exception &ex) {
        ferr() << ex.message() << nl;
        return 1;
    }

    return 0;
}
