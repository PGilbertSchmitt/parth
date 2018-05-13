// Token.h
#pragma once
#include <string>
#include <unordered_map>

typedef std::string token_type;
typedef std::unordered_map<std::string, token_type> keyword_map;

// Tokens are immutable, set once and forget.
class Token
{
private:
	token_type type;
	std::string literal;
	static keyword_map keywords;

public:
	Token();
	Token(token_type, std::string);

	token_type getType();
	std::string getLiteral();
	bool is_empty();

	static token_type lookup_ident(std::string &);

	/* ALL TOKEN TYPES */

	// Special
	const static token_type ILLEGAL;
	const static token_type EOF_VAL;

	// Identifiers and Literals
	const static token_type IDENT;
	const static token_type INT;
	const static token_type STRING;

	// Operators
	const static token_type ASSIGN;
	const static token_type PLUS;
	const static token_type MINUS;
	const static token_type SLASH;
	const static token_type ASTERISK;
	const static token_type BANG;
	const static token_type MODULO;

	// Comparison
	const static token_type LT;
	const static token_type GT;
	const static token_type EQ;
	const static token_type NEQ;
	const static token_type LTEQ;
	const static token_type GTEQ;

	// Delimiters
	const static token_type COMMA;
	const static token_type COLON;
	const static token_type NEWLINE;

	// Keywords
	const static token_type FUNCTION;
	const static token_type LET;
	const static token_type IF;
	const static token_type ELSE;
	const static token_type RETURN;
	const static token_type TRUE_VAL;
	const static token_type FALSE_VAL;

	// Braces
	const static token_type LPAREN;
	const static token_type RPAREN;
	const static token_type LBRACE;
	const static token_type RBRACE;
	const static token_type LBRACKET;
	const static token_type RBRACKET;
};
