#include <stdarg.h>
#include "syntax.hpp"
#include "SyntaxDefinition.hpp"

namespace ftl
{

using namespace syntax;

Definition::Definition(Ref<DebugFactory> debugFactory)
	: debugFactory_(debugFactory),
	  id_(Crc32().sum()),
	  name_(0),
	  caseSensitive_(true),
	  definitionByName_(new DefinitionByName),
	  numRules_(0),
	  numKeywords_(0),
	  ruleByName_(new RuleByName),
	  keywordByName_(new KeywordByName),
	  statefulScope_(false),
	  hasHints_(false),
	  numStateFlags_(0),
	  numStateChars_(0),
	  numStateStrings_(0),
	  flagIdByName_(new StateIdByName),
	  charIdByName_(new StateIdByName),
	  stringIdByName_(new StateIdByName)
{
	if (debugFactory_)
		debugFactory_->definition_ = this;
}

typedef SyntaxDefinition::NODE NODE;

NODE SyntaxDefinition::CHAR(char ch) { return debug(new CharNode(ch, 0), "Char"); }
NODE SyntaxDefinition::OTHER(char ch) { return debug(new CharNode(ch, 1), "Char"); }
NODE SyntaxDefinition::GREATER(char ch) { return debug(new GreaterNode(ch, 0), "Greater"); }
NODE SyntaxDefinition::BELOW(char ch) { return debug(new GreaterNode(ch, 1), "Greater"); }
NODE SyntaxDefinition::GREATER_OR_EQUAL(char ch) { return debug(new GreaterOrEqualNode(ch, 0), "GreaterOrEqual"); }
NODE SyntaxDefinition::BELOW_OR_EQUAL(char ch) { return debug(new GreaterOrEqualNode(ch, 1), "GreaterOrEqual"); }
NODE SyntaxDefinition::ANY() { return debug(new AnyNode(), "Any"); }

NODE SyntaxDefinition::RANGE(char a, char b) { return debug(new RangeMinMaxNode(a, b, 0), "RangeMinMax"); }
NODE SyntaxDefinition::RANGE(const char* s) { return debug(new RangeExplicitNode(s, 0), "RangeExplicit"); }
NODE SyntaxDefinition::EXCEPT(char a, char b) { return debug(new RangeMinMaxNode(a, b, 1), "RangeMinMax"); }
NODE SyntaxDefinition::EXCEPT(const char* s) { return debug(new RangeExplicitNode(s, 1), "RangeExplicit"); }

NODE SyntaxDefinition::STRING(const char* s) { return debug(new StringNode(s, caseSensitive_), "String"); }

NODE SyntaxDefinition::KEYWORD(const char* keywords)
{
	Ref<KeywordMap, Owner> map = new KeywordMap;
	const char* pos = keywords;
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
		int keyword = numKeywords_;
		numKeywords_ += keywordByName_->insert(pos, len, keyword, &keyword);
		map->insert(pos, len, keyword);
		pos += len;
	}
	return debug(new KeywordNode(map, caseSensitive_), "Keyword");
}

NODE SyntaxDefinition::REPEAT(int minRepeat, int maxRepeat, NODE entry) { return debug(new RepeatNode(minRepeat, maxRepeat, entry), "Repeat"); }
NODE SyntaxDefinition::REPEAT(int minRepeat, NODE entry) { return REPEAT(minRepeat, intMax, entry); }
NODE SyntaxDefinition::REPEAT(NODE entry) { return REPEAT(0, intMax, entry); }

NODE SyntaxDefinition::LAZY_REPEAT(int minRepeat, NODE entry) { return debug(new LazyRepeatNode(minRepeat, entry), "LazyRepeat"); }
NODE SyntaxDefinition::LAZY_REPEAT(NODE entry) { return LAZY_REPEAT(0, entry); }

NODE SyntaxDefinition::GREEDY_REPEAT(int minRepeat, int maxRepeat, NODE entry) { return debug(new GreedyRepeatNode(minRepeat, maxRepeat, entry), "GreedyRepeat"); }
NODE SyntaxDefinition::GREEDY_REPEAT(int minRepeat, NODE entry) { return GREEDY_REPEAT(minRepeat, intMax, entry); }
NODE SyntaxDefinition::GREEDY_REPEAT(NODE entry) { return GREEDY_REPEAT(0, intMax, entry); }

NODE SyntaxDefinition::LENGTH(int minLength, int maxLength, NODE entry) { return debug(new LengthNode(minLength, maxLength, entry), "Length"); }
NODE SyntaxDefinition::LENGTH(int minLength, NODE entry) { return LENGTH(minLength, intMax, entry); }
NODE SyntaxDefinition::BOI() { return debug(new BoiNode(), "Boi"); }
NODE SyntaxDefinition::EOI() { return debug(new EoiNode(), "Eoi"); }
NODE SyntaxDefinition::PASS() { return debug(new PassNode(0), "Pass"); }
NODE SyntaxDefinition::FAIL() { return debug(new PassNode(1), "Pass"); }
NODE SyntaxDefinition::FIND(NODE entry) { return debug(new FindNode(entry), "Find"); }
NODE SyntaxDefinition::AHEAD(NODE entry) { return debug(new AheadNode(entry, 0), "Ahead"); }
NODE SyntaxDefinition::NOT(NODE entry) { return debug(new AheadNode(entry, 1), "Ahead"); }
NODE SyntaxDefinition::BEHIND(NODE entry) { return debug(new BehindNode(entry, 0), "Behind"); }
NODE SyntaxDefinition::NOT_BEHIND(NODE entry) { return debug(new BehindNode(entry, 1), "Behind"); }

NODE SyntaxDefinition::CHOICE() { return debug(new ChoiceNode, "Choice"); }
NODE SyntaxDefinition::GLUE() { return debug(new GlueNode, "Glue"); }

NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	node->appendChild(choice10);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	node->appendChild(choice10);
	node->appendChild(choice11);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	node->appendChild(choice10);
	node->appendChild(choice11);
	node->appendChild(choice12);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	node->appendChild(choice10);
	node->appendChild(choice11);
	node->appendChild(choice12);
	node->appendChild(choice13);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	node->appendChild(choice10);
	node->appendChild(choice11);
	node->appendChild(choice12);
	node->appendChild(choice13);
	node->appendChild(choice14);
	return debug(node, "Choice");
}
NODE SyntaxDefinition::CHOICE(NODE choice0, NODE choice1, NODE choice2, NODE choice3, NODE choice4, NODE choice5, NODE choice6, NODE choice7, NODE choice8, NODE choice9, NODE choice10, NODE choice11, NODE choice12, NODE choice13, NODE choice14, NODE choice15) {
	NODE node = new ChoiceNode;
	node->appendChild(choice0);
	node->appendChild(choice1);
	node->appendChild(choice2);
	node->appendChild(choice3);
	node->appendChild(choice4);
	node->appendChild(choice5);
	node->appendChild(choice6);
	node->appendChild(choice7);
	node->appendChild(choice8);
	node->appendChild(choice9);
	node->appendChild(choice10);
	node->appendChild(choice11);
	node->appendChild(choice12);
	node->appendChild(choice13);
	node->appendChild(choice14);
	node->appendChild(choice15);
	return debug(node, "Choice");
}

NODE SyntaxDefinition::GLUE(NODE child0, NODE child1) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	node->appendChild(child10);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	node->appendChild(child10);
	node->appendChild(child11);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	node->appendChild(child10);
	node->appendChild(child11);
	node->appendChild(child12);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	node->appendChild(child10);
	node->appendChild(child11);
	node->appendChild(child12);
	node->appendChild(child13);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	node->appendChild(child10);
	node->appendChild(child11);
	node->appendChild(child12);
	node->appendChild(child13);
	node->appendChild(child14);
	return debug(node, "Glue");
}
NODE SyntaxDefinition::GLUE(NODE child0, NODE child1, NODE child2, NODE child3, NODE child4, NODE child5, NODE child6, NODE child7, NODE child8, NODE child9, NODE child10, NODE child11, NODE child12, NODE child13, NODE child14, NODE child15) {
	NODE node = new GlueNode;
	node->appendChild(child0);
	node->appendChild(child1);
	node->appendChild(child2);
	node->appendChild(child3);
	node->appendChild(child4);
	node->appendChild(child5);
	node->appendChild(child6);
	node->appendChild(child7);
	node->appendChild(child8);
	node->appendChild(child9);
	node->appendChild(child10);
	node->appendChild(child11);
	node->appendChild(child12);
	node->appendChild(child13);
	node->appendChild(child14);
	node->appendChild(child15);
	return debug(node, "Glue");
}

NODE SyntaxDefinition::HINT(const char* text) {
	hasHints_ = true;
	return debug(new HintNode(text), "Hint");
}
NODE SyntaxDefinition::DONE() { return debug(new HintNode(0), "Hint"); }

void SyntaxDefinition::OPTIMIZE()
{
	while (unresolvedLinkHead_) {
		Ref<LinkNode> link = unresolvedLinkHead_;
		link->rule_ = ruleByName(link->ruleName_);
		if (link->rule_->isVoid()) {
			if (Ref<RefNode>(link)) {
				if (link->rule_->numberOfRefs() == 0) {
					Ref<LinkNode, Owner> iLink = new InlineNode(link->ruleName_);
					link->parent()->insertChild(iLink, link);
					iLink->rule_ = link->rule_;
					link->unlink();
				}
			}
		}
		unresolvedLinkHead_ = link->unresolvedNext_;
	}
}

void SyntaxDefinition::LINK(bool optimize)
{
	if (optimize) OPTIMIZE();

	while (unresolvedLinkHead_) {
		unresolvedLinkHead_->rule_ = ruleByName(unresolvedLinkHead_->ruleName_);
		unresolvedLinkHead_ = unresolvedLinkHead_->unresolvedNext_;
	}
	while (unresolvedKeywordHead_) {
		unresolvedKeywordHead_->keyword_ = keywordByName(unresolvedKeywordHead_->keywordName_);
		unresolvedKeywordHead_ = unresolvedKeywordHead_->unresolvedKeywordNext_;
	}
	while (unresolvedInvokeHead_) {
		unresolvedInvokeHead_->definition_ = definitionByName(unresolvedInvokeHead_->definitionName_);
		unresolvedInvokeHead_ = unresolvedInvokeHead_->unresolvedNext_;
	}
	if (!LinkNode::rule_) {
		if (!LinkNode::ruleName_)
			FTL_THROW(DebugException, "Missing entry rule declaration");
		LinkNode::rule_ = ruleByName(LinkNode::ruleName_);
	}
}

NODE SyntaxDefinition::CALL(CallBack callBack, Ref<Instance> self) {
	if (!self) self = this;
	return debug(new CallNode(callBack, self), "Call");
}

NODE SyntaxDefinition::ERROR() {
	return debug(new CallNode(errorCallBack, this), "Call");
}

NODE SyntaxDefinition::SET(const char* name, bool value) {
	return debug(new SetNode(flagIdByName(name), value), "Set");
}
NODE SyntaxDefinition::IF(const char* name, NODE trueBranch, NODE falseBranch) {
	if (!trueBranch) trueBranch = PASS();
	if (!falseBranch) falseBranch = PASS();
	return debug(new IfNode(flagIdByName(name), trueBranch, falseBranch), "If");
}
NODE SyntaxDefinition::GETCHAR(const char* name) {
	return debug(new GetCharNode(charIdByName(name)), "GetChar");
}
NODE SyntaxDefinition::SETCHAR(const char* name, char value) {
	return debug(new SetCharNode(charIdByName(name), value), "SetChar");
}
NODE SyntaxDefinition::VARCHAR(const char* name) {
	return debug(new VarCharNode(charIdByName(name), 0), "VarChar");
}
NODE SyntaxDefinition::VAROTHER(const char* name) {
	return debug(new VarCharNode(charIdByName(name), 1), "VarChar");
}
NODE SyntaxDefinition::GETSTRING(const char* name, NODE coverage) {
	return debug(new GetStringNode(stringIdByName(name), coverage), "GetString");
}
NODE SyntaxDefinition::SETSTRING(const char* name, const char* value) {
	return debug(new SetStringNode(stringIdByName(name), value), "SetString");
}
NODE SyntaxDefinition::VARSTRING(const char* name) {
	return debug(new VarStringNode(stringIdByName(name)), "VarString");
}

NODE SyntaxDefinition::INVOKE(Definition* definition, NODE coverage) {
	return debug(new InvokeNode(definition, coverage), "Invoke");
}

NODE SyntaxDefinition::INVOKE(const char* definitionName, NODE coverage) {
	Ref<InvokeNode, Owner> node = new InvokeNode(definitionName, coverage);
	node->unresolvedNext_ = unresolvedInvokeHead_;
	unresolvedInvokeHead_ = node;
	return debug(node, "Invoke");
}

SyntaxState* SyntaxDefinition::newState(SyntaxState* parent) const
{
	if (!stateful())
		return 0;

	SyntaxState* state = new SyntaxState(id_, numStateFlags_, numStateChars_, numStateStrings_, parent);

	Ref<StateFlag> stateFlag = stateFlagHead_;
	for (int id = numStateFlags_ - 1; id >= 0; --id) {
		*state->flag(id) = stateFlag->defaultValue_;
		stateFlag = stateFlag->next_;
	}

	Ref<StateChar> stateChar = stateCharHead_;
	for (int id = numStateChars_ - 1; id >= 0; --id) {
		*state->character(id) = stateChar->defaultValue_;
		stateChar = stateChar->next_;
	}

	Ref<StateString> stateString = stateStringHead_;
	for (int id = numStateStrings_ - 1; id >= 0; --id) {
		*state->string(id) = stateString->defaultValue_;
		stateString = stateString->next_;
	}

	return state;
}

Ref<Token, Owner> SyntaxDefinition::find(ByteArray* media, int* i0, int* i1, Ref<TokenFactory> tokenFactory) const
{
	int i = *i0;
	Ref<Token, Owner> rootToken;
	while (media->has(i)) {
		if (rootToken = match(media, i, i1, 0, tokenFactory))
			break;
		++i;
	}
	*i0 = i;
	return rootToken;
}

Ref<Token, Owner> SyntaxDefinition::match(ByteArray* media, int i0, int* i1, SyntaxState* state, Ref<TokenFactory> tokenFactory) const
{
	Ref<SyntaxState, Owner> localState;
	if (!state) {
		localState = newState();
		state = localState;
	}

	TokenFactory localTokenFactory;
	if (!tokenFactory)
		tokenFactory = &localTokenFactory;

	int h = matchNext(media, i0, tokenFactory, 0, state);

	if ((i1 != 0) && (h != -1))
		*i1 = h;

	return (h != -1) ? tokenFactory->rootToken() : Ref<Token>();
}

Ref<Definition> SyntaxDefinition::resolveScope(const char*& name) const
{
	Ref<Definition> scope = this;
	int k = 0;
	const char* p0 = name;
	const char* p = p0;
	while (true) {
		char ch = *(p++);
		if (!ch) break;
		k = (ch == ':') ? k + 1 : 0;
		if (k == 2) {
			Ref<Definition, Owner> childScope;
			if (!scope->definitionByName_->lookup(p0, p - p0 - k, &childScope))
				FTL_THROW(DebugException, str::cat("Undefined scope '", name, "' referenced"));
			scope = childScope;
			p0 = p;
			k = 0;
		}
	}
	name = p0;
	return scope;
}

Ref<Definition> SyntaxDefinition::definitionByName(const char* name) const
{
	Ref<Definition, Owner> definition;
	Ref<Definition> scope = resolveScope(name);
	if (!scope->definitionByName_->lookup(name, &definition))
		FTL_THROW(DebugException, str::cat("Undefined definition '", name, "' referenced"));
	return definition;
}

Ref<RuleNode> SyntaxDefinition::ruleByName(const char* name) const
{
	Ref<Definition> scope = resolveScope(name);
	Ref<RuleNode, Owner> node;
	FTL_ASSERT(scope);
	if (!scope->ruleByName_->lookup(name, &node))
		FTL_THROW(DebugException, str::cat("Undefined rule '", name, "' referenced"));
	return node;
}

int SyntaxDefinition::keywordByName(const char* keyword)
{
	int tokenType = -1;
	if (!keywordByName_->lookup(keyword, &tokenType))
		FTL_THROW(DebugException, str::cat("Undefined keyword '", keyword, "' referenced"));
	return tokenType;
}

void SyntaxDefinition::getLineAndPosFromIndex(ByteArray* media, int index, int* line, int* pos)
{
	*line = 1;
	*pos = 1;
	int j = 0;
	while (media->has(j) && (j < index)) {
		char ch = media->get(j);
		if (ch == '\n') { ++*line; *pos = 1; }
		else ++*pos;
		++j;
	}
}

int SyntaxDefinition::syntaxError(ByteArray* media, int index, SyntaxState* state) const
{
	FTL_THROW(DebugException, "Unhandled syntax error");
	return -1;
}

int SyntaxDefinition::flagIdByName(const char* name)
{
	int flagId = -1;
	if (!flagIdByName()->lookup(name, &flagId))
		FTL_THROW(DebugException, str::cat("Undefined state flag '", name, "' referenced"));
	return flagId;
}

int SyntaxDefinition::charIdByName(const char* name)
{
	int charId = -1;
	if (!charIdByName()->lookup(name, &charId))
		FTL_THROW(DebugException, str::cat("Undefined state char '", name, "' referenced"));
	return charId;
}

int SyntaxDefinition::stringIdByName(const char* name)
{
	int stringId = -1;
	if (!stringIdByName()->lookup(name, &stringId))
		FTL_THROW(DebugException, str::cat("Undefined state string '", name, "' referenced"));
	return stringId;
}

int SyntaxDefinition::errorCallBack(Ref<Instance> self, ByteArray* media, int index, SyntaxState* state)
{
	Ref<Definition> definition = self;
	return definition->syntaxError(media, index, state);
}

} // namespace ftl
