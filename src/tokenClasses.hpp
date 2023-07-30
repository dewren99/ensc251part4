//============================================================================
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put NONE if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put �none�)__
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in tokenClasses.cpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P4_<userid1>_<userid2>" (eg. P4_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.hpp
// Description : Course Project Part 4
// Original portions Copyright (c) 2023 School of Engineering Science, Simon Fraser University
//============================================================================

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <exception> // ?
#include <stdexcept>
#include <memory>

#include "TreeNode.hpp"

namespace ensc251 {

char *demangle(const char *typeName);

// forward declarations
class constant;
class Token;

typedef std::shared_ptr<Token> TokenSP;
// global variable external references
extern std::vector<TokenSP> tokenObjectPs; // objects for abstract syntax tree
extern std::size_t tracker; // tracker index walks through tokenObjectPs

// The Token class, parent to all types of tokens
class Token : public TreeNode {	// The Token class is now derived from TreeNode
private:
	int m_lineIndex;
	int m_posIndex;
public:

	int getLineIndex() const { return m_lineIndex; }
	int getPosIndex() const { return m_posIndex; }

	Token(int lineIndex, int posIndex) : m_lineIndex(lineIndex), m_posIndex(posIndex) {}

	virtual const std::string& getStringValue() const = 0;
	virtual bool has_string_value(const std::string& aString) { return getStringValue() == aString; };
	
	void print_ast() { print_traverse(); OUT << ";" << std::endl;}

	friend std::ostream& operator <<(std::ostream& outs, const Token& token){
		token.print(outs);
		return outs;
	}

	void print(std::ostream& outs) const
	{
		outs << demangle(typeid(*this).name()) <<
				" (line: " << m_lineIndex <<", pos: " << m_posIndex <<")" <<
				" : " << this->getStringValue();
	}

	virtual TokenSP process_punctuator(const std::string &puncValue) { return nullptr; }
	virtual TokenSP process_declaration() { return nullptr; }

	virtual TokenSP process_id() { return nullptr; }
	virtual TokenSP process_primary_exp() { return nullptr; }

	virtual TokenSP process_pointer_primary_exp() { return nullptr; } // remove ?

	TokenSP process_numeric_id()	{ return process_id(); }          
	virtual TokenSP process_numeric_primary_exp() { return nullptr; }

	virtual TokenSP process_int_id() { return nullptr; }
	virtual TokenSP process_int_primary_exp() { return nullptr; }

	virtual TokenSP process_int_comp() { return nullptr; }
	virtual TokenSP process_postfix_operator() { return nullptr; }

	virtual TokenSP advance_past_conditional_operator() { return nullptr; }
	virtual TokenSP advance_past_shift_operator() { return nullptr; }

	// TODO: ***** you may need to add code here *****

	virtual TokenSP advance_past_div_operator() { return nullptr; }
	virtual TokenSP advance_past_mod_operator() { return nullptr; }

	virtual TokenSP advance_past_gen_assignment_operator() { return nullptr; }
	virtual TokenSP advance_past_int_assignment_operator() { return nullptr; }

	TokenSP process_token()
	{
		childSPVector.clear();

		++tracker;
		if (tracker < tokenObjectPs.size()){
			return shared_from(this);
		}
		else
			// our grammar must end in ';'. If *this is the end
			//  then it is a premature end which we will report now.
			return nullptr;
	}

	virtual void print_action() { OUT << getStringValue() << " "; }

	virtual void print_traverse()
	{
		if(childSPVector.size() == 2)
			traverse_children_in_order(
					static_cast<traverse_func>(&Token::print_traverse),
					static_cast<action_func>(&Token::print_action));
		else
			traverse_children_pre_order(
					static_cast<traverse_func>(&Token::print_traverse),
					static_cast<action_func>(&Token::print_action));
	}

	virtual ~Token() {}
};

class TokenException : public std::exception {
	std::string ErrMsg;
public:
	TokenException(TokenSP tkn=nullptr, std::string msg=""){
		if(tkn)
			ErrMsg = "\nSyntax Error around {" + tkn->getStringValue() +
					"}, line " + std::to_string(tkn->getLineIndex()) +
					", index " + std::to_string(tkn->getPosIndex()) +
					"\n" + msg + "\n";
		else
			ErrMsg = "Syntax Error!\n" + msg + "\n";
	}
	const char * what () const throw () {
		return ErrMsg.c_str();
	}
};

/*
template<typename T>
class numeric_const;

template<typename T>
class numeric_int_const;
*/ 
class constant : public Token { // subclasses: numeric_const<T>, ...
public:
	constant(int lineIndex, int posIndex): Token(lineIndex, posIndex) {};

	TokenSP process_primary_exp() { return process_token(); }
};

// the below functions are defined in tokenClasses.cpp
TokenSP recursive_parser();
TokenSP assignment_exp();
TokenSP int_assignment_exp();
TokenSP int_comp_exp();

class StringBasedToken:public Token
{
protected:
	std::string m_stringValue;
public:
	StringBasedToken(const std::string &aStringValue, int lineIndex, int posIndex): Token(lineIndex, posIndex), m_stringValue (aStringValue) {};
	void print(std::ostream& str) const { Token::print(str); str << ": " << m_stringValue; } // needed ?
//	bool has_string_value(const std::string& aString) { return m_stringValue == aString; } // override
	const std::string& getStringValue() const override { return m_stringValue; }
//	void print_action() { OUT << m_stringValue << " "; };
};

class punctuator:public StringBasedToken {
	// e.g., ';', '*', '(', ')', ':', etc.
public:
	punctuator(const std::string &puncValue, int lineIndex, int posIndex): StringBasedToken(puncValue, lineIndex, posIndex){ };

	void print_traverse() override
	{
		if (m_stringValue == "(") {
			traverse_children_pre_order(
					static_cast<traverse_func>(&punctuator::print_traverse),
					static_cast<action_func>(&punctuator::print_action));

			OUT << ") ";
		}
		else
			traverse_children_in_order(
					static_cast<traverse_func>(&punctuator::print_traverse),
					static_cast<action_func>(&punctuator::print_action));
	}

	void print_action() override
	{
		StringBasedToken::print_action();
		if (m_stringValue==";")
			OUT << std::endl;
	};

	TokenSP process_primary_exp() override
	{
		if (m_stringValue == "(" && process_token()) {
			if (TokenSP tObjP{assignment_exp()}) {
				if(tokenObjectPs[tracker]->process_punctuator(")")) {
					add_childP(tObjP);
					return shared_from(this);
				}
			}
		}
		return nullptr;
	}

	TokenSP process_int_primary_exp() override
	{
		if(m_stringValue == "(" && process_token())
		{
			TokenSP tObjP = int_assignment_exp();
			if(tObjP)
			{
				if(tokenObjectPs[tracker]->process_punctuator(")"))
					{
					add_childP(tObjP);
					return shared_from(this);
					}
			}
		}
		return nullptr;
	}

	TokenSP process_punctuator(const std::string &puncValue) override
	{
		if (puncValue == m_stringValue)
			return process_token();
		return nullptr;
	}

};

class type_spec:public StringBasedToken
{ // 'char' | 'int' | 'float'
public:
	type_spec(): StringBasedToken("int", 0, 0) { };
	type_spec(const std::string &typeValue, int lineIndex, int posIndex): StringBasedToken(typeValue, lineIndex, posIndex){ };
	// TODO: ***** Complete this class
	TokenSP process_declaration() override
	{
		if(process_token())
		{
			if(TokenSP tObjP{tokenObjectPs[tracker]->process_id()})
			{
				add_childP(tObjP);
				return shared_from(this);
			}
		}
		return nullptr;
	}
	/*
	TokenSP process_int_id() override
	{
		if(process_token())
		{
			if(TokenSP tObjP{tokenObjectPs[tracker]->process_id()})
			{
				add_childP(tObjP);
				return shared_from(this);
			}
		}
		return nullptr;
	}

	TokenSP process_pointer_primary_exp() override
	{
		if(process_token())
		{
			if(TokenSP tObjP{tokenObjectPs[tracker]->process_id()})
			{
				add_childP(tObjP);
				return shared_from(this);
			}
		}
		return nullptr;
	}

	TokenSP process_int_primary_exp() override
	{
		if(process_token())
		{
			if(TokenSP tObjP{tokenObjectPs[tracker]->process_id()})
			{
				add_childP(tObjP);
				return shared_from(this);
			}
		}
		return nullptr;
	}

	TokenSP process_int_comp() override
	{
		if(process_token())
		{
			if(TokenSP tObjP{tokenObjectPs[tracker]->process_id()})
			{
				add_childP(tObjP);
				return shared_from(this);
			}
		}
		return nullptr;
	}
	*/
};

class id:public StringBasedToken
{	// identifiers -- example:  sum
public:
	id(const std::string &idValue, int lineIndex, int posIndex): StringBasedToken(idValue, lineIndex, posIndex) {  };
	TokenSP process_id() override	{ return process_token(); }
	TokenSP process_primary_exp() override { return process_id(); }
};

class pointer_id:public id {	// id of type char*
public:
	pointer_id(const std::string &idValue, int lineIndex, int posIndex): id(idValue, lineIndex, posIndex) {  };
};

class numeric_id:public id {	// id of type float (and see below)
public:
	numeric_id(const std::string &idValue, int lineIndex, int posIndex): id(idValue, lineIndex, posIndex) {  };
	TokenSP process_numeric_primary_exp() override { return process_numeric_id(); }
};

class int_id:public numeric_id {	// id of types char and int
public:
	int_id(const std::string &idValue, int lineIndex, int posIndex): numeric_id(idValue, lineIndex, posIndex) {  };
	TokenSP process_int_id() override	{ return process_id(); } // a different pattern is being followed here.
	TokenSP process_int_primary_exp() override { return process_int_id(); }
};

class gen_assignment_operator:public StringBasedToken
{
	// '=' | '/=' | '+=' | '-='
public:
	gen_assignment_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex) { };
	TokenSP advance_past_gen_assignment_operator() { return process_token();	}
};

class int_assignment_operator:public StringBasedToken
{
	// '%=' | '<<=' | '>>='
public:
	int_assignment_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex) { };
	TokenSP advance_past_int_assignment_operator() { return process_token();	}
};

class conditional_operator:public StringBasedToken
{
	// ?
public:
	conditional_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex){ };
	TokenSP advance_past_conditional_operator() {
		return process_token();
	}
	void print_traverse() override
	{
		static_cast<Token*>(this->childSPVector[0].get())->print_traverse();
		this->print_action();
		static_cast<Token*>(this->childSPVector[1].get())->print_traverse();
		OUT << ": " ;
		static_cast<Token*>(this->childSPVector[2].get())->print_traverse();
	}
};

class shift_operator:public StringBasedToken
{
	// '<<' | '>>'
public:
	shift_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex){ };
	TokenSP advance_past_shift_operator()
	{
		return process_token();
	}
};

class additive_operator:public StringBasedToken
{
	// '+' | '-'
public:
	additive_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex){ };
	// TODO: ***** Complete this class if needed
	TokenSP process_int_comp() override
	{
		TokenSP subTreeP;
		if(process_token() && (subTreeP = int_comp_exp()))
		{
			add_childP(subTreeP);
			return shared_from(this);
		}
		return nullptr;
	}
};

class div_operator:public StringBasedToken {	// '/'
public:
	div_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex) { };
	TokenSP advance_past_div_operator() { return process_token(); }
};

class mod_operator:public StringBasedToken {	// '%'
public:
	mod_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex) { };
	TokenSP advance_past_mod_operator() { return process_token(); }
};

class comp_operator:public StringBasedToken
{
	//  '~'
public:
	comp_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex){ };

	TokenSP process_int_comp() override
	{
		TokenSP subTreeP;
		if(process_token() && (subTreeP = int_comp_exp()))
		{
			add_childP(subTreeP);
			return shared_from(this);
		}
		return nullptr;
	}
};

class postfix_operator:public StringBasedToken
{	// '++' | '--'
public:
	postfix_operator(const std::string &opValue, int lineIndex, int posIndex): StringBasedToken(opValue, lineIndex, posIndex){ };
	// TODO: ***** Complete this class if needed
	TokenSP process_postfix_operator() override { return process_token(); }
	void print_traverse() override
	{
		traverse_children_post_order(
							static_cast<traverse_func>(&punctuator::print_traverse),
							static_cast<action_func>(&punctuator::print_action));
	}
};

class string:public StringBasedToken
{
	// e.g., "sample string"
public:
	string(const std::string &aStringValue, int lineIndex, int posIndex) : StringBasedToken(aStringValue, lineIndex, posIndex) {};

	TokenSP process_primary_exp() override
	{
		return process_token();
	}
};

class incorrect:public StringBasedToken
{
	// e.g., "sample string     --> This is incorrect ...
	//		(quotation mark " is missing at the end)
public:
	incorrect(const std::string &aStringValue, int lineIndex, int posIndex) : StringBasedToken(aStringValue, lineIndex, posIndex) {};
};

template<typename T>
class numeric_const:public constant
{	// e.g., 123 | 'a' | 12.5
private:
	T m_value;
	std::string m_stringValue;
public:
	numeric_const(const T& constValue, int lineIndex, int posIndex): constant(lineIndex, posIndex), m_value(constValue), m_stringValue(std::to_string(m_value))  {}

	const std::string& getStringValue() const override { return m_stringValue; }
	const T& getValue() const { return m_value; }
	void print(std::ostream& outs) const {Token::print(outs); outs << ": " << m_value; }
	void print_action() override {
		if (std::is_same<T, char>::value)
			OUT << "'" << m_value << "'" << " ";
		else
			OUT << m_value << " ";
	};

	TokenSP process_numeric_primary_exp() override { return this->process_primary_exp(); }
};

template<typename T>
class int_const:public numeric_const<T>
{	// e.g., 123 | 'a'
public:
	int_const(const T& constValue, int lineIndex, int posIndex): numeric_const<T>(constValue, lineIndex, posIndex) {}

	TokenSP process_int_primary_exp() override { return this->process_primary_exp(); }
};

} // namespace ensc251
