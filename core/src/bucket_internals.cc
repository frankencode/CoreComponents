/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/bucket_internals>
#include <cc/Format>

namespace cc {
namespace bucket {

String Internals::dotifyTree(const Tree *tree)
{
    Format format;
    format
        << "digraph g {\n"
        << "graph [\n"
        << "rankdir = \"LR\";\n"
        << "]\n";
    dotifyNode(format, tree->root());
    format << "}\n";
    return format;
}

void Internals::dotifyNode(Format &format, const Head *head, unsigned origin)
{
    Weight weight = head->weight_;
    const Node *node = head->node_;

    if (node->isBranch_) {
        format
            << "branch_" << (void *)node << " [\n"
            << "label = \"<f0>origin: " << origin << "|<f1>weight: " << weight << "|<f2>fill: " << node->fill_ << "|<f3>succ: " << (void *)node->succ_ << "\"\n"
            << "shape = \"record\"\n"
            << "];\n";

        const Branch *branch = static_cast<const Branch *>(node);
        for (unsigned i = 0; i < branch->fill_; ++i) {
            dotifyNode(format, &branch->at(i), i);
        }

        #if 0
        if (!branch->at(0).node_->isBranch_) {
            format << "{ rank=same; ";
            for (unsigned i = 0; i < branch->fill_; ++i) {
                Node *child = branch->at(i).node_;
                format << (child->isBranch_ ? "branch_" : "node_") << (void *)child << "; " << nl;
            }
            format << "}\n";
        }
        #endif

        for (unsigned i = 0; i < branch->fill_; ++i) {
            Node *child = branch->at(i).node_;
            format << "branch_" << (void *)node << " -> " << (child->isBranch_ ? "branch_" : "node_") << (void *)child << ":f0 [weight=1000];" << nl;
        }
    }
    else {
        format
            << "node_" << (void *)node << " [\n"
            << "label = \"<f0>origin: " << origin << "|<f1>weight: " << weight << "|<f2>fill: " << node->fill_ << "|<f3>succ: " << (void *)node->succ_ << "\"\n"
            << "shape = \"record\"\n"
            << "];\n";

        /*if (node->succ_)
            format << "node_" << (void *)node << ":f3 -> node_" << (void *)node->succ_ << ":f3 [weight=0.001];" << nl;*/
    }
}

}} // namespace cc::bucket
