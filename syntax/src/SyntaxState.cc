/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/syntax>
#include <cc/syntax/SyntaxDefinition>
#include <cc/syntax/TokenFactory>
#include <cc/syntax/SyntaxState>

namespace cc {
namespace syntax {

SyntaxState::SyntaxState(const DefinitionNode *definition, int numFlags, int numCaptures, TokenFactory *tokenFactory):
    definition_(definition),
    flags_(Flags::create(numFlags)),
    captures_(Captures::create(numCaptures)),
    tokenFactory_(tokenFactory),
    hint_(0),
    hintOffset_(-1),
    finalize_(false)
{
    if (!tokenFactory_) tokenFactory_ = new TokenFactory;
    for (int i = 0; i < flags_->count(); ++i) flags_->at(i) = false;
}

Token *SyntaxState::produceToken(int scope, int rule, const char *scopeName, const char *ruleName)
{
    Token *token = tokenFactory_->produce(scope, rule, scopeName, ruleName);
    if (!rootToken_) rootToken_ = token;
    return token;
}

bool SyntaxState::flag(const char *name) const
{
    const DefinitionNode *scope = definition_->resolveScope(name);
    int flagId = scope->flagIdByName(name);
    if (scope == definition_) return flags_->at(flagId);
    Ref<SyntaxState> state;
    if (!stateByScope_->lookup(scope, &state)) return false; // FIXME: should throw debug error
    return state->flags_->at(flagId);
}

Range SyntaxState::capture(const char *name) const
{
    const DefinitionNode *scope = definition_->resolveScope(name);
    int captureId = scope->captureIdByName(name);
    if (scope == definition_) return captures_->at(captureId);
    Ref<SyntaxState> state;
    if (!stateByScope_->lookup(scope, &state)) return Range{}; // FIXME: should throw debug error
    return state->captures_->at(captureId);
}

bool SyntaxState::lookupCapture(const char *name, Range *range) const
{
    const DefinitionNode *scope = definition_->resolveScope(name);
    int captureId = -1;
    if (!scope->lookupCaptureIdByName(name, &captureId)) return false;
    CC_ASSERT(captureId != -1);
    if (scope == definition_) {
        *range = captures_->at(captureId);
        return true;
    }
    Ref<SyntaxState> state;
    if (!stateByScope_->lookup(scope, &state)) return false;
    *range = state->captures_->at(captureId);
    return true;
}

SyntaxState *SyntaxState::stateByScope(const DefinitionNode *scope)
{
    if (scope == definition_) return this;
    if (!stateByScope_) stateByScope_ = StateByScope::create();
    Ref<SyntaxState> state;
    if (!stateByScope_->lookup(scope, &state)) {
        state = scope->createState(tokenFactory_);
        stateByScope_->insert(scope, state);
    }
    return state;
}

}} // namespace cc::syntax
