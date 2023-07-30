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
//% * Enter the above information in tokenClasses.hpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P4_<userid1>_<userid2>" (eg. P4_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.cpp
// Description : Course Project Part 4
// Original portions Copyright (c) 2023 School of Engineering Science, Simon Fraser University
//============================================================================

#include "tokenClasses.hpp"

// I'm not sure if next line works with other than GNU compiler
#include <cxxabi.h>

using namespace std;

namespace ensc251 {

// global variables
std::vector<TokenSP> tokenObjectPs; // objects for abstract syntax tree
std::size_t tracker{0}; // tracker index

char *demangle(const char *typeName) {
	int status;
	// I'm not sure if the below line works with compilers other than GNU
	return abi::__cxa_demangle(typeName, 0, 0, &status);
}

/**
 * @brief postfix_exp first checks for an id followed by a postfix_operator.
 * If there exists a postfix operator then it becomes the root of the subtree with id as a child.
 * If not exist check for the primary expression and retrieving its sub tree.

	postfix_exp : id postfix_operator
				| primary_exp
				;

	int_postfix_exp : int_id postfix_operator
					| int_primary_exp
					;

	postfix_operator : '++' | '--'
					 ;
 *
 * @return TokenSP
 */
TokenSP postfix_exp()
{
	// TODO: ***** Complete this function
	std::size_t old_tracker{tracker};

	auto subTreeP{tokenObjectPs[tracker]->process_id()};
	if(subTreeP)
	{
		if(TokenSP tObjP{tokenObjectPs[tracker]->process_postfix_operator()})
		{
			tObjP->add_childP(subTreeP);
			return tObjP;
		}
		else
			return nullptr;
	}

	tracker = old_tracker;
	subTreeP = tokenObjectPs[tracker]->process_primary_exp();
	// TODO: ***** Complete this function
	return subTreeP;
}

TokenSP int_postfix_exp()
{
	// TODO: ***** Complete this function
	std::size_t old_tracker{tracker};

	auto subTreeP{tokenObjectPs[tracker]->process_int_id()};
	if(subTreeP)
	{
		std::cout << "[int_postfix_exp] int_id postfix_operator" << std::endl;
		if(TokenSP tObjP{tokenObjectPs[tracker]->process_postfix_operator()})
		{
			tObjP->add_childP(subTreeP);
				return tObjP;
		}
		else
			return nullptr;
	}

	tracker = old_tracker;
	std::cout << "[int_postfix_exp] process_int_primary_exp" << std::endl;
	subTreeP = tokenObjectPs[tracker]->
	// TODO: ***** Complete this function
											process_int_primary_exp();
	//};
	return subTreeP;
}

TokenSP comp_exp()
{
	std::size_t old_tracker{tracker};

	TokenSP tObjP{tokenObjectPs[tracker]->process_int_comp()};

	if(!tObjP) {
		tracker = old_tracker;
		tObjP = postfix_exp();
	}
	return tObjP;
}

TokenSP int_comp_exp()
{
	std::size_t old_tracker{tracker};

	TokenSP tObjP{tokenObjectPs[tracker]->process_int_comp()};
	std::cout << "[int_comp_exp] process_int_comp" << std::endl;
	if(!tObjP) {
		std::cout << "[int_comp_exp] int_postfix_exp" << std::endl;
		tracker = old_tracker;
		tObjP = int_postfix_exp();
	}
	return tObjP;
}

TokenSP int_div_exp()
{
	TokenSP lowerNodeP{int_comp_exp()};
	if (lowerNodeP) {
		std::cout << "[int_div_exp] int_comp_exp" << std::endl;
		for (TokenSP upperNodeP{nullptr};
				(upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()) ||
				(upperNodeP = tokenObjectPs[tracker]->advance_past_mod_operator());)
			if(TokenSP ueTreeP{int_comp_exp()}) {
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(ueTreeP);
				lowerNodeP = upperNodeP;
			}
			else
				return nullptr;
	}
	return lowerNodeP;
}

TokenSP div_exp()
{
	std::size_t old_tracker{tracker};

	TokenSP lowerNodeP{comp_exp()};
	if (lowerNodeP) {
		std::cout << "[div_exp] comp_exp" << std::endl;
		while (TokenSP upperNodeP{tokenObjectPs[tracker]->advance_past_div_operator()}) {
			if(TokenSP ueTreeP{comp_exp()}) {
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(ueTreeP);
				lowerNodeP = upperNodeP;
			}
			else
				return nullptr;
		}
	}
	else {
		std::cout << "[div_exp] int_div_exp" << std::endl;
		tracker = old_tracker;
		lowerNodeP = int_div_exp();
	}

	return lowerNodeP;
}

// TODO: ***** Add more functions around here somewhere *****
/**
 * @brief
additive_exp can also be represented as 'div_exp {additive_operator div_exp}*'
where * means zero or more instances of { additive_operator div_exp }


	additive_exp 	: div_exp {additive_operator div_exp}*
					;

	int_additive_exp : int_div_exp {additive_operator int_div_exp}*
					 ;

	additive_operator : '+' | '-'
					  ;
 *
 * @return TokenSP
 */
TokenSP additive_exp()
{
	TokenSP lowerNodeP{div_exp()};
	if (lowerNodeP) {
		std::cout << "[additive_exp] div_exp" << std::endl;
		while (TokenSP upperNodeP{tokenObjectPs[tracker]->advance_past_additive_operator()}) {
			if(TokenSP ueTreeP{div_exp()}) {
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(ueTreeP);
				lowerNodeP = upperNodeP;
			}
			else
				return nullptr;
		}
	}
	return lowerNodeP;
}

TokenSP int_additive_exp()
{
	TokenSP lowerNodeP{int_div_exp()};
	if (lowerNodeP) {
		std::cout << "[int_additive_exp] int_div_exp" << std::endl;
		while (TokenSP upperNodeP{tokenObjectPs[tracker]->advance_past_additive_operator()}) {
			if(TokenSP ueTreeP{int_div_exp()}) {
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(ueTreeP);
				lowerNodeP = upperNodeP;
			}
			else
				return nullptr;
		}
	}
	return lowerNodeP;
}

TokenSP shift_exp()
{
	// TODO: ***** Fix and complete this function
	TokenSP lowerNodeP{int_additive_exp()};

	if (lowerNodeP) {
		std::cout << "[shift_exp] int_div_exp" << std::endl;
		while (TokenSP upperNodeP{tokenObjectPs[tracker]->advance_past_shift_operator()})
		{
			// TODO: ***** Fix and complete this code
			if(TokenSP subTreeP{int_additive_exp()}) {
			// TODO: ***** complete this code
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(subTreeP);
				lowerNodeP = upperNodeP;
			}
			else
				return nullptr;
		}
	}
	return lowerNodeP;
}

TokenSP ternary_exp()
{
	std::size_t old_tracker{tracker};
	TokenSP subTreeP    ;

	// TODO: ***** Complete this function
	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		std::cout << "[ternary_exp] id '?' assignment_exp ':' ternary_exp" << std::endl;
		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_conditional_operator()})
		{
			tObjP->add_childP(subTreeP);
			subTreeP = assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}

	std::cout << "[ternary_exp] shift_exp" << std::endl;
	tracker = old_tracker;
	subTreeP = shift_exp();

	if(!subTreeP)
	{
		tracker = old_tracker;
		// TODO: ***** Fix and complete this function
		std::cout << "[ternary_exp] div_exp" << std::endl;
		subTreeP = additive_exp();
	}
	return subTreeP;
}

TokenSP int_ternary_exp()
{
	std::size_t old_tracker{tracker};
	TokenSP subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_conditional_operator()})
		{
			tObjP->add_childP(subTreeP);
			subTreeP = int_assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = int_ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}
	tracker = old_tracker;
	return shift_exp();
}

TokenSP assignment_exp()
{
	std::size_t old_tracker{tracker};

	// int_id int_assignment_operator int_assignment_exp
	TokenSP subTreeP{tokenObjectPs[tracker]->process_int_id()};
	if(subTreeP)
	{
		std::cout << "[assignment_exp] int_id int_assignment_operator int_assignment_exp" << std::endl;
		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_int_assignment_operator()})
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production: id gen_assignment_operator assignment_exp
	tracker = old_tracker;
	if ((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		std::cout << "[assignment_exp] id gen_assignment_operator assignment_exp" << std::endl;
		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_gen_assignment_operator()})
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				// we have assignment operator, but not valid assignment expression,
				//	so there must be an error and it should be reported.
				return nullptr;
		}
		// no assignment operator, so this is not this production

		// start production:  ternary_exp
		if(tokenObjectPs[tracker]->has_string_value(";")){
			std::cout << "[assignment_exp] ternary_exp, we have already processed an id" << std::endl;
			// we have already processed an id, which satisfies the ternary_exp production.
			return subTreeP;
		}
	}

	// production:  ternary_exp
	std::cout << "[assignment_exp] ternary_exp" << std::endl;
	tracker = old_tracker;
	return ternary_exp();
}

TokenSP int_assignment_exp()
{
	std::size_t old_tracker{tracker};

	TokenSP subTreeP{tokenObjectPs[tracker]->process_int_id()};
	if(subTreeP)
	{
		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_gen_assignment_operator()})
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}

		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_int_assignment_operator()})
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production:  int_ternary_exp
	tracker = old_tracker;
	return int_ternary_exp();
}

TokenSP stat()
{
	if(auto subTreeP{tokenObjectPs[tracker]->process_declaration()}){
		std::cout << "processing it as declaration" << std::endl;
		return subTreeP;
	}
	else {
		std::cout << "processing it as assignment expression" << std::endl;
		return assignment_exp();
	}
}

// because the below function deals with ";", it is a bit different than the pattern seen elsewhere.
TokenSP stat_list()
{
	if(auto lowerNodeP{stat()}) {

		while (tokenObjectPs[tracker]->has_string_value(";")) {
			tracker++;
			if (tracker == tokenObjectPs.size())
				return lowerNodeP;
			TokenSP upperNodeP{tokenObjectPs[tracker - 1]};

			if(TokenSP statTreeP{stat()}) {
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(statTreeP);
				lowerNodeP = upperNodeP;			}
			else
				throw ensc251::TokenException(tokenObjectPs[tracker]);
		}
	}
	throw ensc251::TokenException(tokenObjectPs[tracker]);
}

TokenSP recursive_parser()
{
	if (tokenObjectPs.size())
		return stat_list();
	else
		throw ensc251::TokenException();
}

}

