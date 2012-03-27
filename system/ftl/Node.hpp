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
	Node(int type, String source, Ref<Token> token)
		: type_(type),
		  source_(source),
		  token_(token)
	{}

	enum Type { AtomType = 0, ArrayType = 1, ObjectType = 2 };
	inline int type() const { return type_; }
	inline bool isAtom() const { return type_ == AtomType; }
	inline bool isArray() const { return type_ == ArrayType; }
	inline bool isObject() const { return type_ == ObjectType; }

	inline String source() const { return source_; }
	inline Ref<Token> token() const { return token_; }
	inline String text() const { return text(token_); }
	inline void linePos(int* line, int* pos) { source_->offsetToLinePos(token_->i0(), line, pos); }

protected:
	static String text(Ref<Token> token) { return source_->copy(token->i0(), token->i1()); }

private:
	int type_;
	String source_;
	Ref<Token, Owner> token_;
};

class ArrayNode: public Node
{
public:
	ArrayNode(int size, String source, Ref<Token> token)
		: Node(Node::ArrayType, source, token),
		  items_(new Items(size))
	{}

	inline Ref<Node> item(int index) { return items_->get(index); }
	inline void setItem(int index, Ref<Node> node) { items_->set(index, node); }

private:
	typedef Array< Ref<Node, Owner> > Items;
	Ref<Items, Owner> items_;
};

class ObjectNode: public Node
{
public:
	ObjectNode(String name, String type, String source, Ref<Token> token)
		: Node(Node::ObjectType, source, token),
		  name_(name),
		  type_(type),
		  members_(new Members)
	{}

	inline String name() const { return name_; }
	inline String type() const { return type_; }

	inline int memberCount() const { return members_->length(); }
	inline Ref<Node> member(int index) { return members_->get(index); }
	inline Ref<Node> member(String name) { return members_->value(name); }

	inline bool insertMember(String name, Ref<Node> node) { return members_->insert(name, node); }

private:
	typedef Map<String, Ref<Node, Owner> > Members;

	String name_;
	String type_;
	Ref<Members, Owner> members_;
};

} // namespace ftl

#endif // FTL_NODE_HPP
