/*
 * Node.hpp -- generic parsed data structure
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_NODE_HPP
#define FTL_NODE_HPP

#include "Token.hpp"
#include "String.hpp"
#include "Array.hpp"
#include "Map.hpp"

namespace ftl
{

class Node: public Instance
{
public:
	enum Type { AtomType, ArrayType, ObjectType };

	Node(Ref<ByteArray> source, Ref<Token> token, int type = AtomType)
		: type_(type),
		  source_(source),
		  token_(token)
	{}

	inline int type() const { return type_; }
	inline bool isAtom() const { return type_ == AtomType; }
	inline bool isArray() const { return type_ == ArrayType; }
	inline bool isObject() const { return type_ == ObjectType; }

	inline Ref<ByteArray> source() const { return source_; }
	inline Ref<Token> token() const { return token_; }
	inline String text() const { return text(token_); }
	inline void linePos(int* line, int* pos) { source_->offsetToLinePos(token_->i0(), line, pos); }

protected:
	inline String text(Ref<Token> token) const { return source_->copy(token->i0(), token->i1()); }

private:
	int type_;
	Ref<ByteArray, Owner> source_;
	Ref<Token, Owner> token_;
};

class ArrayNode: public Node
{
public:
	ArrayNode(int size, Ref<ByteArray> source, Ref<Token> token)
		: Node(source, token, Node::ArrayType),
		  items_(new Items(size))
	{}

	inline int itemCount() const { return items_->length(); }
	inline Ref<Node> item(int index) const { return items_->get(index); }
	inline void setItem(int index, Ref<Node> node) { items_->set(index, node); }

private:
	typedef Array< Ref<Node, Owner> > Items;
	Ref<Items, Owner> items_;
};

class ObjectNode: public Node
{
public:
	ObjectNode(String type, Ref<ByteArray> source, Ref<Token> token)
		: Node(source, token, Node::ObjectType),
		  type_(type),
		  memberNames_(new StringList),
		  members_(new Members)
	{}

	inline String type() const { return type_; }

	inline int memberCount() const { return members_->length(); }
	inline String memberName(int index) const { return memberNames_->at(index); }
	inline Ref<Node> member(String name) const { return members_->value(name); }

	inline bool insertMember(String name, Ref<Node, Owner> node) {
		memberNames_->append(name);
		return members_->insert(name, node);
	}

private:
	typedef Map<String, Ref<Node, Owner> > Members;

	String type_;
	Ref<StringList, Owner> memberNames_;
	Ref<Members, Owner> members_;
};

} // namespace ftl

#endif // FTL_NODE_HPP
