#include "token.h"

// Constructor for empty tokens
Token::Token() {
  type = TokenType::NONE;
  literal = "";
}

Token::Token(TokenType _type, std::string _literal, uint _column, uint _line) {
  type = _type;
  literal = _literal;
  column = _column;
  line = _line;
}

TokenType Token::get_type() const { return type; }

std::string Token::get_literal() const { return literal; }

uint Token::get_column() const { return column; }

uint Token::get_line() const { return line; }

bool Token::is_empty() const {
  return type == TokenType::NONE && literal == "";
}

TokenType Token::lookup_ident(std::string &ident) {
  keyword_map::const_iterator got = keywords.find(ident);
  if (got != keywords.end()) {
    return got->second;
  }
  if (ident.back() == '?') {
    return TokenType::OPTION;
  }
  return TokenType::IDENT;
}

// Used for lookup of keywords in Token::lookup_ident
keyword_map Token::keywords({
    {"let", TokenType::LET},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"return", TokenType::RETURN},
    {"true", TokenType::TRUE_VAL},
    {"false", TokenType::FALSE_VAL},
});