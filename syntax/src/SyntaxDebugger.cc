/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/syntax/SyntaxDebugger>

namespace cc {
namespace syntax {

static void printChar(char ch) {
    if (ch < ' '/*32*/) {
        if (ch == '\t')
            fout() << "\\t";
        else if (ch == '\n')
            fout() << "\\n";
        else if (ch == '\f')
            fout() << "\\f";
        else if (ch == '\r')
            fout() << "\\r";
        else if (ch == 0)
            fout() << "\\0";
        else
            fout("\\%%") << oct(ToAscii<char>::map(ch));
    }
    else if (ch < 127) {
        if (ch == '\\')
            fout() << "\\";
        fout() << ToAscii<char>::map(ch);
    }
    else {
        fout("\\%%") << oct(ToUnicode<char>::map(ch));
    }
}

static void printString(const char *s, int size) {
    for (int i = 0; i < size; ++i) {
        char ch = s[i];
        if (ch == '"')
            fout() << "\\";
        printChar(s[i]);
    }
}

static void printString(const CharArray &s) {
    printString(s.chars(), s.count());
}

static void printCharAttr(char ch) {
    fout() << "'";
    if (ch == '\'')
        fout() << "\\";
    printChar(ch);
    fout() << "'";
}

static void printStringAttr(const CharArray &s) {
    fout() << "\"";
    printString(s);
    fout() << "\"";
}

void SyntaxDebugNode::printNext(String indent)
{
    fout() << indent << declType() << "(";
    printAttributes(Format() << indent << debugger_->indent_);
    fout() << ")";
}

void SyntaxDebugNode::printBranch(SyntaxNode *node, String indent) {
    if (node) {
        SyntaxDebugNode *debugNode = Object::cast<SyntaxDebugNode *>(node);
        if (debugNode) debugNode->printNext(indent);
        else fout() << indent;
    }
    else {
        fout("%%PASS()") << indent;
    }
}

String SyntaxDebugNode::superIndent(String indent) const {
    return (indent != "") ? indent->copy(0, indent->count() - debugger_->indent_->count()) : indent;
}

String SyntaxDebugNode::subIndent(String indent) const {
    return Format() << indent << SyntaxDebugNode::debugger_->indent_;
}

class CharDebugNode: public SyntaxDebugNode {
public:
    CharDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return charNode()->invert() ? "OTHER" : "CHAR"; }

    virtual void printAttributes(String indent) {
        printCharAttr(charNode()->ch());
    }

private:
    inline CharNode *charNode() const { return Object::cast<CharNode *>(SyntaxDebugNode::entry()); }
};

class GreaterDebugNode: public SyntaxDebugNode {
public:
    GreaterDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return greaterNode()->invert() ? "BELOW" : "GREATER"; }

    virtual void printAttributes(String index) {
        printCharAttr(greaterNode()->ch());
    }

private:
    inline GreaterNode *greaterNode() const { return Object::cast<GreaterNode *>(SyntaxDebugNode::entry()); }
};

class GreaterOrEqualDebugNode: public SyntaxDebugNode {
public:
    GreaterOrEqualDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return greaterOrEqualNode()->invert() ? "BELOW_OR_EQUAL" : "GREATER_OR_EQUAL"; }

    virtual void printAttributes(String index) {
        printCharAttr(greaterOrEqualNode()->ch());
    }

private:
    inline GreaterOrEqualNode *greaterOrEqualNode() const { return Object::cast<GreaterOrEqualNode *>(SyntaxDebugNode::entry()); }
};

class AnyDebugNode: public SyntaxDebugNode {
public:
    AnyDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "ANY"; }
};

class RangeMinMaxDebugNode: public SyntaxDebugNode {
public:
    RangeMinMaxDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return rangeMinMaxNode()->invert() ? "EXCEPT" : "RANGE"; }

    virtual void printAttributes(String indent) {
        RangeMinMaxNode *node = rangeMinMaxNode();
        printCharAttr(node->a());
        fout() << ", ";
        printCharAttr(node->b());
    }

private:
    inline RangeMinMaxNode *rangeMinMaxNode() const { return Object::cast<RangeMinMaxNode *>(SyntaxDebugNode::entry()); }
};

class RangeExplicitDebugNode: public SyntaxDebugNode {
public:
    RangeExplicitDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return rangeExplicitNode()->invert() ? "EXCEPT" : "RANGE"; }

    virtual void printAttributes(String indent) {
        printStringAttr(rangeExplicitNode()->s());
    }

private:
    inline RangeExplicitNode *rangeExplicitNode() const { return Object::cast<RangeExplicitNode *>(SyntaxDebugNode::entry()); }
};

class StringDebugNode: public SyntaxDebugNode {
public:
    StringDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "STRING"; }

    virtual void printAttributes(String indent) {
        printStringAttr(stringNode()->s());
    }

private:
    inline StringNode *stringNode() const { return Object::cast<StringNode *>(SyntaxDebugNode::entry()); }
};

class KeywordDebugNode: public SyntaxDebugNode {
public:
    KeywordDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "KEYWORD"; }

    virtual void printAttributes(String indent) {
        fout("\n%%\"") << indent;
        KeywordMap *map = keywordNode()->map();
        for (KeywordMap::Index index = map->first(); map->has(index); ++index) {
            Ref<KeywordMap::Key> s = map->key(index);
            printString(*s);
            if (index != map->last())
                fout(" ");
        }
        fout("\"\n") << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline KeywordNode *keywordNode() const { return Object::cast<KeywordNode *>(SyntaxDebugNode::entry()); }
};

class RepeatDebugNode: public SyntaxDebugNode {
public:
    RepeatDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "REPEAT"; }

    virtual void printAttributes(String indent) {
        RepeatNode *node = repeatNode();
        if (node->maxRepeat() != intMax)
            fout("%%, %%,") << node->minRepeat() << node->maxRepeat();
        else if (node->minRepeat() != 0)
            fout("%%,") << node->minRepeat();
        fout() << "\n";
        printBranch(node->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline RepeatNode *repeatNode() const { return Object::cast<RepeatNode *>(SyntaxDebugNode::entry()); }
};

class LazyRepeatDebugNode: public SyntaxDebugNode {
public:
    LazyRepeatDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "LAZY_REPEAT"; }

    virtual void printAttributes(String indent) {
        LazyRepeatNode *node = repeatNode();
        if (node->minRepeat() != 0)
            fout("%%,") << node->minRepeat();
        fout() << "\n";
        printBranch(node->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline LazyRepeatNode *repeatNode() const { return Object::cast<LazyRepeatNode *>(SyntaxDebugNode::entry()); }
};

class GreedyRepeatDebugNode: public SyntaxDebugNode {
public:
    GreedyRepeatDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "GREEDY_REPEAT"; }

    virtual void printAttributes(String indent) {
        GreedyRepeatNode *node = repeatNode();
        if (node->maxRepeat() != intMax)
            fout("%%, %%,") << node->minRepeat() << node->maxRepeat();
        else if (node->minRepeat() != 0)
            fout("%%,") << node->minRepeat();
        fout() << "\n";
        printBranch(node->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline GreedyRepeatNode *repeatNode() const { return Object::cast<GreedyRepeatNode *>(SyntaxDebugNode::entry()); }
};

class FilterDebugNode: public SyntaxDebugNode {
public:
    FilterDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "FILTER"; }

    virtual void printAttributes(String indent) {
        FilterNode *node = filterNode();
        fout() << "\n";
        printBranch(node->filter(), indent);
        fout(",");
        printCharAttr(node->blank());
        fout(",\n");
        printBranch(node->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline FilterNode *filterNode() const { return Object::cast<FilterNode *>(SyntaxDebugNode::entry()); }
};

class LengthDebugNode: public SyntaxDebugNode {
public:
    LengthDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "LENGTH"; }

    virtual void printAttributes(String indent) {
        LengthNode *node = lengthNode();
        if (node->maxLength() != intMax)
            fout("%%, %%,\n") << node->minLength() << node->maxLength();
        else
            fout("%%,\n") << node->minLength();
        printBranch(node->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline LengthNode *lengthNode() const { return Object::cast<LengthNode *>(SyntaxDebugNode::entry()); }
};

class BoiDebugNode: public SyntaxDebugNode {
public:
    BoiDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "BOI"; }
};

class EoiDebugNode: public SyntaxDebugNode {
public:
    EoiDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "EOI"; }
};

class PassDebugNode: public SyntaxDebugNode {
public:
    PassDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode)
        : SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return passNode()->invert() ? "FAIL" : "PASS"; }

private:
    inline PassNode *passNode() const { return Object::cast<PassNode *>(SyntaxDebugNode::entry()); }
};

class FindDebugNode: public SyntaxDebugNode {
public:
    FindDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "FIND"; }

    virtual void printAttributes(String indent) {
        fout() << "\n";
        printBranch(findNode()->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline FindNode *findNode() const { return Object::cast<FindNode *>(SyntaxDebugNode::entry()); }
};

class AheadDebugNode: public SyntaxDebugNode {
public:
    AheadDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return aheadNode()->invert() ? "NOT" : "AHEAD"; }

    virtual void printAttributes(String indent) {
        fout() << "\n";
        printBranch(aheadNode()->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline AheadNode *aheadNode() const { return Object::cast<AheadNode *>(SyntaxDebugNode::entry()); }
};

class BehindDebugNode: public SyntaxDebugNode {
public:
    BehindDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return behindNode()->invert() ? "NOT_BEHIND" : "BEHIND"; }

    virtual void printAttributes(String indent) {
        fout() << "\n";
        printBranch(behindNode()->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline BehindNode *behindNode() const { return Object::cast<BehindNode *>(SyntaxDebugNode::entry()); }
};

class ChoiceDebugNode: public SyntaxDebugNode {
public:
    ChoiceDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "CHOICE"; }

    virtual void printAttributes(String indent) {
        fout() << "\n";
        SyntaxNode *node = choiceNode()->firstChoice();
        while (node) {
            printBranch(node, indent);
            node = node->nextSibling();
            if (node) fout() << ",\n";
        }
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline ChoiceNode *choiceNode() const { return Object::cast<ChoiceNode *>(SyntaxDebugNode::entry()); }
};

class GlueDebugNode: public SyntaxDebugNode {
public:
    GlueDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "GLUE"; }

    virtual void printAttributes(String indent) {
        fout() << "\n";
        SyntaxNode *node = glueNode()->firstChild();
        while (node) {
            printBranch(node, indent);
            node = node->nextSibling();
            if (node) fout() << ",\n";
        }
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline GlueNode *glueNode() const { return Object::cast<GlueNode *>(SyntaxDebugNode::entry()); }
};

class HintDebugNode: public SyntaxDebugNode {
public:
    HintDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return hintNode()->strict() ? "EXPECT" : "HINT"; }

    virtual void printAttributes(String indent) {
        HintNode *node = hintNode();
        fout("\"%%\",\n") << node->message();
        printBranch(node->entry(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline HintNode *hintNode() const { return Object::cast<HintNode *>(SyntaxDebugNode::entry()); }
};

class RefDebugNode: public SyntaxDebugNode {
public:
    RefDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return refNode()->generate() ? "REF" : "INLINE"; }

    virtual void printAttributes(String indent) {
        fout("\"%%\"") << refNode()->ruleName();
    }

private:
    inline RefNode *refNode() const { return Object::cast<RefNode *>(SyntaxDebugNode::entry()); }
};

class InvokeDebugNode: public SyntaxDebugNode {
public:
    InvokeDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "INVOKE"; }

    virtual void printAttributes(String indent) {
        fout("\"%%\"") << invokeNode()->ruleName();
    }

private:
    inline InvokeNode *invokeNode() const { return Object::cast<InvokeNode *>(SyntaxDebugNode::entry()); }
};

class PreviousDebugNode: public SyntaxDebugNode {
public:
    PreviousDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "PREVIOUS"; }

    virtual void printAttributes(String indent) {
        PreviousNode *node = previousNode();
        fout("\"%%\"") << node->ruleName();
        if (node->keywordName())
            fout(", \"%%\"") << node->keywordName();
    }

private:
    inline PreviousNode *previousNode() const { return Object::cast<PreviousNode *>(SyntaxDebugNode::entry()); }
};

class ContextDebugNode: public SyntaxDebugNode {
public:
    ContextDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "CONTEXT"; }

    virtual void printAttributes(String indent) {
        ContextNode *node = contextNode();
        fout("\"%%\",\n") << node->ruleName();
        printBranch(node->inContext(), indent);
        fout() << ",\n";
        printBranch(node->outOfContext(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline ContextNode *contextNode() const { return Object::cast<ContextNode *>(SyntaxDebugNode::entry()); }
};

class CallDebugNode: public SyntaxDebugNode {
public:
    CallDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "CALL"; }

    virtual void printAttributes(String indent) {
        fout("0x%hex:8.:'0'") << (void *)callNode()->callBack();
    }

private:
    inline CallNode *callNode() const { return Object::cast<CallNode *>(SyntaxDebugNode::entry()); }
};

class SetDebugNode: public SyntaxDebugNode {
public:
    SetDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "SET"; }

    virtual void printAttributes(String indent) {
        SetNode *node = setNode();
        fout("\"%%\", %%") << SyntaxDebugNode::debugger_->flagNameById()->value(node->flagId()) << node->value();
    }

private:
    inline SetNode *setNode() const { return Object::cast<SetNode *>(SyntaxDebugNode::entry()); }
};

class IfDebugNode: public SyntaxDebugNode {
public:
    IfDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "IF"; }

    virtual void printAttributes(String indent) {
        IfNode *node = ifNode();
        fout("\"%%\",\n") << SyntaxDebugNode::debugger_->flagNameById()->value(node->flagId());
        printBranch(node->trueBranch(), indent);
        fout() << ",\n";
        printBranch(node->falseBranch(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline IfNode *ifNode() const { return Object::cast<IfNode *>(SyntaxDebugNode::entry()); }
};

class CaptureDebugNode: public SyntaxDebugNode {
public:
    CaptureDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "CAPTURE"; }

    virtual void printAttributes(String indent) {
        CaptureNode *node = captureNode();
        fout("\"%%\",\n") << SyntaxDebugNode::debugger_->captureNameById()->value(node->captureId());
        printBranch(node->coverage(), indent);
        fout() << "\n" << SyntaxDebugNode::superIndent(indent);
    }

private:
    inline CaptureNode *captureNode() const { return Object::cast<CaptureNode *>(SyntaxDebugNode::entry()); }
};

class ReplayDebugNode: public SyntaxDebugNode {
public:
    ReplayDebugNode(SyntaxDebugger *debugger, SyntaxNode *newNode):
        SyntaxDebugNode(debugger, newNode)
    {}

    virtual const char *declType() const { return "REPLAY"; }

    virtual void printAttributes(String indent) {
        fout("\"%%\"") << SyntaxDebugNode::debugger_->captureNameById()->value(replayNode()->captureId());
    }
private:
    inline ReplayNode *replayNode() const { return Object::cast<ReplayNode *>(SyntaxDebugNode::entry()); }
};

SyntaxDebugger::SyntaxDebugger(String indent):
    factoryByNodeType_(FactoryByNodeType::create()),
    indent_(indent)
{
    factoryByNodeType_->insert("Char",           new DebugNodeFactory<CharDebugNode>           (this));
    factoryByNodeType_->insert("Greater",        new DebugNodeFactory<GreaterDebugNode>        (this));
    factoryByNodeType_->insert("GreaterOrEqual", new DebugNodeFactory<GreaterOrEqualDebugNode> (this));
    factoryByNodeType_->insert("Any",            new DebugNodeFactory<AnyDebugNode>            (this));
    factoryByNodeType_->insert("RangeMinMax",    new DebugNodeFactory<RangeMinMaxDebugNode>    (this));
    factoryByNodeType_->insert("RangeExplicit",  new DebugNodeFactory<RangeExplicitDebugNode>  (this));
    factoryByNodeType_->insert("String",         new DebugNodeFactory<StringDebugNode>         (this));
    factoryByNodeType_->insert("Keyword",        new DebugNodeFactory<KeywordDebugNode>        (this));
    factoryByNodeType_->insert("Repeat",         new DebugNodeFactory<RepeatDebugNode>         (this));
    factoryByNodeType_->insert("LazyRepeat",     new DebugNodeFactory<LazyRepeatDebugNode>     (this));
    factoryByNodeType_->insert("GreedyRepeat",   new DebugNodeFactory<GreedyRepeatDebugNode>   (this));
    factoryByNodeType_->insert("Length",         new DebugNodeFactory<LengthDebugNode>         (this));
    factoryByNodeType_->insert("Eoi",            new DebugNodeFactory<EoiDebugNode>            (this));
    factoryByNodeType_->insert("Boi",            new DebugNodeFactory<BoiDebugNode>            (this));
    factoryByNodeType_->insert("Pass",           new DebugNodeFactory<PassDebugNode>           (this));
    factoryByNodeType_->insert("Find",           new DebugNodeFactory<FindDebugNode>           (this));
    factoryByNodeType_->insert("Ahead",          new DebugNodeFactory<AheadDebugNode>          (this));
    factoryByNodeType_->insert("Behind",         new DebugNodeFactory<BehindDebugNode>         (this));
    factoryByNodeType_->insert("Choice",         new DebugNodeFactory<ChoiceDebugNode>         (this));
    factoryByNodeType_->insert("Glue",           new DebugNodeFactory<GlueDebugNode>           (this));
    factoryByNodeType_->insert("Hint",           new DebugNodeFactory<HintDebugNode>           (this));
    factoryByNodeType_->insert("Ref",            new DebugNodeFactory<RefDebugNode>            (this));
    factoryByNodeType_->insert("Invoke",         new DebugNodeFactory<InvokeDebugNode>         (this));
    factoryByNodeType_->insert("Previous",       new DebugNodeFactory<PreviousDebugNode>       (this));
    factoryByNodeType_->insert("Context",        new DebugNodeFactory<ContextDebugNode>        (this));
    factoryByNodeType_->insert("Call",           new DebugNodeFactory<CallDebugNode>           (this));
    factoryByNodeType_->insert("Set",            new DebugNodeFactory<SetDebugNode>            (this));
    factoryByNodeType_->insert("If",             new DebugNodeFactory<IfDebugNode>             (this));
    factoryByNodeType_->insert("Capture",        new DebugNodeFactory<CaptureDebugNode>        (this));
    factoryByNodeType_->insert("Replay",         new DebugNodeFactory<ReplayDebugNode>         (this));
}

void SyntaxDebugger::printDefinition(bool omitUnusedRules)
{
    if (omitUnusedRules)
        determineRulesInUse(SyntaxDebugFactory::definition()->rule());

    typedef DefinitionNode::RuleByName RuleByName;
    RuleByName *ruleByName = SyntaxDebugFactory::definition()->ruleByName_;

    typedef Map<int, RuleNode *> RuleById;
    Ref<RuleById> ruleById = RuleById::create();

    for (RuleByName::Index i = ruleByName->first(); ruleByName->has(i); ++i) {
        RuleNode *rule = ruleByName->value(i);
        ruleById->insert(rule->id(), rule);
    }

    for (int i = 0; i < ruleById->count(); ++i) {
        RuleNode *rule = ruleById->valueAt(i);
        if (omitUnusedRules && !rule->used()) continue;
        fout("DEFINE(\"%%\",\n") << rule->name();
        if (rule->entry()) {
            SyntaxDebugNode *debugNode = Object::cast<SyntaxDebugNode *>(rule->entry());
            if (debugNode) debugNode->printNext(indent_);
        }
        else {
            fout("%%PASS()") << indent_;
        }
        fout() << "\n);\n\n";
    }

    fout("ENTRY(\"%%\");\n") << SyntaxDebugFactory::definition()->ruleName_;
}

SyntaxNode *SyntaxDebugger::produce(SyntaxNode *newNode, const char *nodeType)
{
    Ref<NodeFactory> factory;
    return factoryByNodeType_->lookup(nodeType, &factory) ? factory->produce(newNode) : newNode;
}

Ref<SyntaxDebugger::StateNameById> SyntaxDebugger::newReverseMap(StateIdByName *stateIdByName)
{
    Ref<StateNameById> stateNameById = StateNameById::create();
    for (StateIdByName::Index i = stateIdByName->first(); stateIdByName->has(i); ++i) {
        String name = stateIdByName->key(i);
        int id = stateIdByName->value(i);
        stateNameById->insert(id, name);
    }
    return stateNameById;
}

SyntaxDebugger::StateNameById *SyntaxDebugger::flagNameById() {
    if (!flagNameById_)
        flagNameById_ = newReverseMap(this->definition()->flagIdByName_);
    return flagNameById_;
}

SyntaxDebugger::StateNameById *SyntaxDebugger::captureNameById() {
    if (!captureNameById_)
        captureNameById_ = newReverseMap(this->definition()->captureIdByName_);
    return captureNameById_;
}

void SyntaxDebugger::determineRulesInUse(RuleNode *rule)
{
    if (!rule->used()) {
        rule->markUsed();
        for (SyntaxNode *node: rule->entry()->inOrder()) {
            LinkNode *link = Object::cast<LinkNode *>(node);
            if (link) determineRulesInUse(link->rule());
        }
    }
}

}} // namespace cc::syntax
