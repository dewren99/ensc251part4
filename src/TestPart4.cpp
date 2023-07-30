/*
 * TestPart4.cpp
 *
 *  Created on: Summer 2017
 *      Author: Mohammad Akbari
 *
 *  Copyright (c) 2017 School of Engineering Science, Simon Fraser University
 */

#include <iostream>
#include <sstream>

#include "tokenClassifier.hpp"

using namespace std;

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 4
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;

void check(std::istream& input, std::istream& expectedOutput)
{
	std::cout <<  "\n*** " << boost::unit_test::framework::current_test_case().p_name << " ***\n";
	std::string strExpectedOutput;
	try{
		ensc251::tokenObjectPs = tokenClassifier(input);
		auto ast_root = ensc251::recursive_parser();
		if(ast_root)
		{
			ast_root->print_ast();
			std::string strOuput;
			istringstream output(OUT.str());
			while(output >> strOuput && expectedOutput >> strExpectedOutput)
				if(strExpectedOutput!=strOuput){
					std::cout << strExpectedOutput << " != " << strOuput << std::endl;
					BOOST_ERROR("FAILED! \noutput not matched in ("+strOuput+")");
					break;
				}
		}
	}
	catch(ensc251::TokenException& e) 	{
		expectedOutput >> strExpectedOutput;
		if(strExpectedOutput=="SyntaxError")
			std::cout<<e.what();
		else
		{
			std::cout<<"FAILED!\n";
			BOOST_ERROR(e.what());
		}
	}
	OUT.str(""); // Clear the stream for the next test case!
	ensc251::tracker = 0; // Reset the tracker for the next case!
}

BOOST_AUTO_TEST_CASE(test1)
{
	istringstream input("errno=5;");
	istringstream expectedOutput ("errno = 5 ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test2)
{
	istringstream input ("int a; a=5;");
	istringstream expectedOutput ("int a ; "
								  "a = 5 ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test3)
{
	istringstream input ("errno ++;" );
	istringstream expectedOutput ("errno ++ ;" );
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test4)
{
	istringstream input("float f; f=6/4.5/3+4;");
	istringstream expectedOutput ("float f ; "
								  "f = 6 / 4.5 / 3 + 4 ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test5)
{
	istringstream input ("errno>>=10+~errno;");
	istringstream expectedOutput ("errno >>= 10 + ~ errno ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test6)
{
	istringstream input ("char *c;errno>> 2 <<(errno/2 ) ;");
	istringstream expectedOutput ("char * c ; "
								  "errno >> 2 << ( errno / 2 ) ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test7)
{
	istringstream input ("char chr;errno = errno-'2'+chr;");
	istringstream expectedOutput ("char chr ; "
								  "errno = errno - '2' + chr ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test8)
{
	istringstream input ("errno = ((25+errno)-1) / (errno+++2);");
	istringstream expectedOutput ("errno = ( ( 25 + errno ) - 1 ) / ( errno ++ + 2 ) ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test9)
{
	istringstream input ("int a; int c; errno %= a?1:c ;");
	istringstream expectedOutput ("int a ; "
								  "int c ; "
								  "errno %= a ? 1 : c ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test10)
{
	istringstream input ("float f; f = errno ? errno ? 5.5:2:3 ;");
	istringstream expectedOutput ("float f ; "
								  "f = errno ? errno ? 5.5 : 2 : 3 ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test11)
{
	istringstream input ("int i ; i %= (i=2.5)+(i<<=2) ;");
	istringstream expectedOutput ("int i ; "
								  "i %= ( i = 2.5 ) + ( i <<= 2 ) ;");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test12)
{
	istringstream input ("int b; \na=10;");
	istringstream expectedOutput ("int b ; a = 10 ;");
	check(input, expectedOutput);
}

// Syntax Error
BOOST_AUTO_TEST_CASE(testE1)
{
	istringstream input ("errno 10;");
	istringstream expectedOutput ("SyntaxError");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testE2)
{
	istringstream input ("float a; errno >>= a?a:2 ;");
	istringstream expectedOutput ("SyntaxError");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testE3)
{
	istringstream input ("float f; f = 5.. ;");
	istringstream expectedOutput ("SyntaxError");
	check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testE4)
{
	istringstream input ("float ;");
	istringstream expectedOutput ("SyntaxError");
	check(input, expectedOutput);
}
