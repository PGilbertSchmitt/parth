#pragma once
#include <string>

enum class TokenType : int {
  // Extra special, represents a null token, useful for tokens that aren't
  // illegal, but where no other token makes sense
  NONE = -1,

  // Special
  ILLEGAL,
  EOF_VAL,

  // Identifiers and Literals
  IDENT,
  INT,
  STRING,

  // Operators
  ASSIGN,
  PLUS,
  MINUS,
  SLASH,
  ASTERISK,
  BANG,
  MODULO,

  // Comparison
  LT,
  GT,
  EQ,
  NEQ,
  LTEQ,
  GTEQ,

  // Delimiters
  COMMA,
  COLON,
  NEWLINE,

  // Keywords
  FUNCTION,
  LET,
  IF,
  ELSE,
  RETURN,
  TRUE_VAL,
  FALSE_VAL,

  // Braces
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET
};

std::string token_type_string(TokenType type);