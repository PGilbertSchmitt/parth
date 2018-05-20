// Token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>
#include "token_type.h"

typedef std::unordered_map<std::string, TokenType> keyword_map;

// Tokens are immutable, set once and forget.
class Token {
 private:
  TokenType type;
  std::string literal;
  uint column;
  uint line;

  static keyword_map keywords;

 public:
  Token();
  Token(TokenType _type, std::string _literal, uint _column, uint _line);

  TokenType getType();
  std::string getLiteral();
  uint getColumn();
  uint getLine();
  bool is_empty();

  static TokenType lookup_ident(std::string &ident);
};

#endif