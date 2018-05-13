#include "token.h"

// Constructor for empty tokens
Token::Token()
{
	type = "";
	literal = "";
}

Token::Token(token_type _type, std::string _literal, uint _column, uint _line)
{
	type = _type;
	literal = _literal;
	column = _column;
	line = _line;
}

// Used for lookup of keywords in Token::lookup_ident
keyword_map Token::keywords({
		{"fn", Token::FUNCTION},
		{"let", Token::LET},
		{"if", Token::IF},
		{"else", Token::ELSE},
		{"return", Token::RETURN},
		{"true", Token::TRUE_VAL},
		{"false", Token::FALSE_VAL},
});

token_type Token::getType()
{
	return type;
}

std::string Token::getLiteral()
{
	return literal;
}

uint Token::getColumn()
{
	return column;
}

uint Token::getLine()
{
	return line;
}

bool Token::is_empty()
{
	return type == "" && literal == "";
}

token_type Token::lookup_ident(std::string &ident)
{
	keyword_map::const_iterator got = keywords.find(ident);
	if (got != keywords.end())
	{
		return got->second;
	}
	return ident;
}

/* ALL TOKEN TYPES */

// Special
const token_type Token::ILLEGAL = "ILLEGAL";
const token_type Token::EOF_VAL = "EOF";

// Identifiers and Literals
const token_type Token::IDENT = "IDENT";
const token_type Token::INT = "INT";
const token_type Token::STRING = "STRING";

// Operators
const token_type Token::ASSIGN = "=";
const token_type Token::PLUS = "+";
const token_type Token::MINUS = "-";
const token_type Token::SLASH = "/";
const token_type Token::ASTERISK = "*";
const token_type Token::BANG = "!";
const token_type Token::MODULO = "%";

// Comparison
const token_type Token::LT = "<";
const token_type Token::GT = ">";
const token_type Token::EQ = "==";
const token_type Token::NEQ = "!=";
const token_type Token::LTEQ = "<=";
const token_type Token::GTEQ = ">=";

// Delimiters
const token_type Token::COMMA = ",";
const token_type Token::COLON = ":";
const token_type Token::NEWLINE = "\n";

// Keywords
const token_type Token::FUNCTION = "FUNCTION";
const token_type Token::LET = "LET";
const token_type Token::IF = "IF";
const token_type Token::ELSE = "ELSE";
const token_type Token::RETURN = "RETURN";
const token_type Token::TRUE_VAL = "TRUE";
const token_type Token::FALSE_VAL = "FALSE";

// Braces
const token_type Token::LPAREN = "(";
const token_type Token::RPAREN = ")";
const token_type Token::LBRACE = "{";
const token_type Token::RBRACE = "}";
const token_type Token::LBRACKET = "[";
const token_type Token::RBRACKET = "]";