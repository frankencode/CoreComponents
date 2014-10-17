/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SYNTAXDEBUGGER_H
#define FLUX_SYNTAXDEBUGGER_H

#include <flux/syntax>
#include <flux/String>
#include <flux/Map>

namespace flux {

namespace syntax {

class DebugNode: public Node {
public:
    virtual int matchNext(ByteArray *text, int i, Token *parentToken, SyntaxState *state) const {
        return entry()->matchNext(text, i, parentToken, state);
    }

    virtual Node *succ(Node *node) const {
        return Node::parent() ? Node::parent()->succ(Node::self()) : null<Node>();
    }

    virtual int matchLength() const {
        return entry() ? entry()->matchLength() : -1;
    }

    virtual const char *declType() const = 0;
    virtual void printAttributes(String indent) {}

    virtual void printNext(String indent = "");

    inline Node *entry() const { return Node::firstChild(); }

protected:
    DebugNode(Debugger *debugger, Node *newNode)
        : debugger_(debugger)
    {
        appendChild(newNode);
    }

    void printBranch(Node *node, String indent);

    String superIndent(String indent) const;
    String subIndent(String indent) const;

    Debugger *debugger_;
};

class Debugger: public DebugFactory
{
public:
    inline static Ref<Debugger> create(String indent = "\t") {
        return new Debugger(indent);
    }

    virtual Node *produce(Node *newNode, const char *nodeType);
    void printDefinition(bool omitUnusedRules = false);

    typedef DefinitionNode::StateIdByName StateIdByName;
    typedef Map<int, String> StateNameById;

    Ref<StateNameById> newReverseMap(StateIdByName *stateIdByName);
    StateNameById *flagNameById();
    StateNameById *captureNameById();

private:
    friend class DefinitionNode;
    friend class DebugNode;

    Debugger(String indent);

    static void determineRulesInUse(RuleNode *rule);

    class NodeFactory: public Object {
    public:
        virtual Node *produce(Node *newNode) = 0;
    };

    template<class DebugNodeType>
    class DebugNodeFactory: public NodeFactory {
    public:
        DebugNodeFactory(Debugger *debugger)
            : debugger_(debugger)
        {}
        virtual Node *produce(Node *newNode) {
            return new DebugNodeType(debugger_, newNode);
        }
    private:
        Debugger *debugger_;
    };

    typedef PrefixTree< char, Ref<NodeFactory> > FactoryByNodeType;
    Ref<FactoryByNodeType> factoryByNodeType_;

    String indent_;

    Ref<StateNameById> flagNameById_;
    Ref<StateNameById> captureNameById_;
};

} // namespace syntax

typedef syntax::DebugNode SyntaxDebugNode;
typedef syntax::Debugger SyntaxDebugger;

} // namespace flux

#endif // FLUX_SYNTAXDEBUGGER_H
