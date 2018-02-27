/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/SyntaxDebugFactory>
#include <cc/syntax/syntax>

namespace cc {
namespace syntax {

int RuleNode::matchNext(const CharArray *text, int i, Token *parentToken, SyntaxState *state) const
{
    Ref<Token> token = state->produceToken(scope_->id(), id_, scope_->name(), name_);
    if (parentToken)
        parentToken->appendChild(token);

    int i0 = i;
    i = (entry()) ? entry()->matchNext(text, i, token, state) : i;

    if (token) {
        if (i != -1)
            token->setRange(i0, i);
        else
            token->unlink();
    }

    return i;
}

int InvokeNode::matchNext(const CharArray *text, int i, Token *parentToken, SyntaxState *state) const
{
    int i0 = i;
    Token *lastChildSaved = parentToken->lastChild();

    if (coverage()) {
        i = coverage()->matchNext(text, i, parentToken, state);
        rollBack(parentToken, lastChildSaved);
        if (i == -1) return i;
    }

    Ref<SyntaxState> childState = LinkNode::rule()->scope()->createState(state->tokenFactory_);
    i = RefNode::matchNext(text->select(i0, coverage() ? i : text->count()), 0, parentToken, childState);

    if (childState->hint_) {
        state->hint_ = childState->hint_;
        state->hintOffset_ = childState->hintOffset_ + i0;
    }

    if (i == -1) return i;

    if (i0 != 0) {
        i += i0;
        for (
            Token *token = lastChildSaved ? lastChildSaved->nextSibling() : parentToken->firstChild();
            token;
            token = token->nextSibling()
        )
            shiftTree(token, i0);
    }

    return i;
}

void InvokeNode::shiftTree(Token *root, int delta)
{
    root->setRange(root->i0() + delta, root->i1() + delta);
    for (Token *token = root->firstChild(); token; token = token->nextSibling())
        shiftTree(token, delta);
}

NODE DefinitionNode::KEYWORD(const char *keywords)
{
    Ref<KeywordMap> map = KeywordMap::create();
    const char *pos = keywords;
    while (*pos) {
        if ((*pos == ' ') || (*pos == '\t')) {
            ++pos;
            continue;
        }
        int len = 0;
        while (true) {
            char ch = *(pos + len);
            if ((ch == ' ') || (ch == '\t') || (ch == '\0')) break;
            ++len;
        }
        int keyword = keywordCount_;
        keywordCount_ += keywordByName_->insert(pos, len, keyword, &keyword);
        map->insert(pos, len, keyword);
        pos += len;
    }
    return debug(new KeywordNode(map, caseSensitive_), "Keyword");
}

void DefinitionNode::LINK()
{
    if (LinkNode::rule_) return;
    while (unresolvedLinkHead_) {
        unresolvedLinkHead_->rule_ = ruleByName(unresolvedLinkHead_->ruleName_);
        unresolvedLinkHead_ = unresolvedLinkHead_->unresolvedNext_;
    }
    while (unresolvedKeywordHead_) {
        unresolvedKeywordHead_->keyword_ = keywordByName(unresolvedKeywordHead_->keywordName_);
        unresolvedKeywordHead_ = unresolvedKeywordHead_->unresolvedKeywordNext_;
    }
    if (!LinkNode::ruleName_)
        CC_DEBUG_ERROR("Missing entry rule declaration");
    LinkNode::rule_ = ruleByName(LinkNode::ruleName_);
}

Ref<SyntaxState> DefinitionNode::find(const CharArray *text, int i, TokenFactory *tokenFactory) const
{
    Ref<SyntaxState> state = createState(tokenFactory);
    while (text->has(i)) {
        int h = matchNext(text, i, 0, state);
        if (h == -1) state->rootToken_ = 0;
        else break;
        ++i;
    }
    return state;
}

Ref<SyntaxState> DefinitionNode::match(const CharArray *text, int i, TokenFactory *tokenFactory) const
{
    Ref<SyntaxState> state = createState(tokenFactory);
    int h = i < 0 ? 0 : i;
    h = matchNext(text, h, 0, state);
    if ( (h == -1) || (i < 0 && h < text->count()) ) state->rootToken_ = 0;
    return state;
}

const DefinitionNode *DefinitionNode::resolveScope(const char *&name) const
{
    const DefinitionNode *scope = this;
    int k = 0;
    const char *p0 = name;
    const char *p = p0;
    while (true) {
        char ch = *(p++);
        if (!ch) break;
        k = (ch == ':') ? k + 1 : 0;
        if (k == 2) {
            Ref<const DefinitionNode> childScope;
            if (!scope->scopeByName_->lookup(p0, p - p0 - k, &childScope))
                CC_DEBUG_ERROR(Format("Undefined scope '%%'") << name);
            scope = childScope;
            p0 = p;
            k = 0;
        }
    }
    name = p0;
    return scope;
}

int DefinitionNode::syntaxError(const CharArray *text, int index, SyntaxState *state) const
{
    CC_DEBUG_ERROR("Unhandled syntax error");
    return -1;
}

int DefinitionNode::errorCallBack(Object *self, const CharArray *text, int index, Token *parentToken, SyntaxState *state)
{
    DefinitionNode *definition = Object::cast<DefinitionNode *>(self);
    return definition->syntaxError(text, index, state);
}

}} // namespace cc::syntax
