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

TokenSP postfix_exp()
{
	// TODO: ***** Complete this function

	auto subTreeP{tokenObjectPs[tracker]->process_id()};
	// TODO: ***** Complete this function
	return subTreeP;
}

TokenSP int_postfix_exp()
{
	// TODO: ***** Complete this function

	auto subTreeP = tokenObjectPs[tracker]->
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

	if(!tObjP) {
		tracker = old_tracker;
		tObjP = int_postfix_exp();
	}
	return tObjP;
}

TokenSP int_div_exp()
{
	TokenSP lowerNodeP{int_comp_exp()};
	if (lowerNodeP) {
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
		tracker = old_tracker;
		lowerNodeP = int_div_exp();
	}

	return lowerNodeP;
}

// TODO: ***** Add more functions around here somewhere *****

TokenSP shift_exp()
{
	// TODO: ***** Fix and complete this function
	TokenSP lowerNodeP{int_div_exp()};

	if (lowerNodeP) {
		while (TokenSP upperNodeP{tokenObjectPs[tracker]->advance_past_shift_operator()})
		{
			// TODO: ***** Fix and complete this code
			if(TokenSP subTreeP{int_div_exp()}) {
			// TODO: ***** complete this code
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

	tracker = old_tracker;
	subTreeP = shift_exp();

	if(!subTreeP)
	{
		tracker = old_tracker;
		// TODO: ***** Fix and complete this function
		subTreeP = div_exp();
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
		if(tokenObjectPs[tracker]->has_string_value(";"))
			// we have already processed an id, which satisfies the ternary_exp production.
			return subTreeP;
	}

	// production:  ternary_exp
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
	if(auto subTreeP{tokenObjectPs[tracker]->process_declaration()})
		return subTreeP;
	else
		return assignment_exp();
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

