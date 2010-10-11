#ifndef QUERYGRAMMAR_HPP_
#define QUERYGRAMMAR_HPP_

#include "boost/spirit/include/qi.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "AST.hpp"

namespace VerifyTAPN
{
	using boost::spirit::ascii::space_type;
	using boost::spirit::qi::rule;
	using boost::spirit::qi::grammar;

	template<class Iterator>
	struct QueryGrammar : grammar<Iterator, space_type>
	{
		typedef rule<Iterator, space_type> grammar_rule;
		QueryGrammar() : QueryGrammar::base_type(query, "query")
		{

			using boost::spirit::qi::lit;
			using boost::spirit::qi::int_;
			using boost::spirit::qi::char_;
			using boost::spirit::ascii::string;
			using boost::spirit::qi::on_error;
			using boost::spirit::qi::fail;
			using boost::phoenix::construct;
			using boost::phoenix::val;

			query 						= efQuery | agQuery;
			efQuery 					= string("EF") >> parExpression;
			agQuery 					= string("AG") >> parExpression;
			parExpression		 		%= '(' >> orExpression >> ')';
			orExpression 				%= andExpression >> orOp >> andExpression;
			andExpression		 		%= atomicPropOrParExpression >> andOp >> atomicPropOrParExpression;
			atomicPropOrParExpression	= atomicProposition | parExpression;
			atomicProposition 			%= (identifier >> relationOp >> int_);
			relationOp 					%= string("<") | string("<=") | string("=") | string(">=") | string(">");
			andOp						= lit("and") | lit("&&");
			orOp						= lit("or") | lit("||");
			identifier					%= char_("a-zA-Z_") >> *char_("a-zA-Z_0-9");

			query.name("query");
			efQuery.name("efQuery");
			agQuery.name("agQuery");
			parExpression.name("parExpression");
			orExpression.name("orExpression");
			andExpression.name("andExpression");
			atomicPropOrParExpression.name("atomicPropOrParExpression");
			atomicProposition.name("atomicProposition");
			relationOp.name("relationOp");
			andOp.name("andOp");
			orOp.name("orOp");
			identifier.name("identifier");

			using boost::spirit::qi::_2;
			using boost::spirit::qi::_3;
			using boost::spirit::qi::_4;

			on_error<fail>(query,
					std::cout << val("Error! Expecting ")
					<< _4
					<< val(" here: \"")
					<< construct<std::string>(_3, _2)
					<< val("\"")
					<< std::endl
			);
		};

		grammar_rule query;
		rule<Iterator, AST::Query(), space_type> efQuery;
		rule<Iterator, AST::Query(), space_type> agQuery;
		rule<Iterator, AST::ParExpression(), space_type> parExpression;
		rule<Iterator, AST::OrExpression(), space_type> orExpression;
		rule<Iterator, AST::AndExpression(), space_type> andExpression;
		grammar_rule atomicPropOrParExpression;
		rule<Iterator, AST::AtomicProposition(), space_type> atomicProposition;
		rule<Iterator, std::string(), space_type> relationOp;
		rule<Iterator, space_type> andOp;
		rule<Iterator, space_type> orOp;
		rule<Iterator, std::string(), space_type> identifier;
	};
}

#endif /* QUERYGRAMMAR_HPP_ */
