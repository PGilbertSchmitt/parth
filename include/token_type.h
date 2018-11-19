#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <string>

enum class TokenType : int {
  // Extra special, represents a null token, useful for tokens that aren't
  // illegal, but where no other token makes sense (such as a block expression)
  NONE = -1,

  // Special
  ILLEGAL,
  EOF_VAL,

  // Identifiers and Literals
  IDENT,
  INT,
  STRING,
  OPTION,

  // Operators
  ASSIGN,
  PLUS,
  MINUS,
  SLASH,
  ASTERISK,
  BANG,
  MODULO,
  AMP,
  PIPE,
  CARET,
  DOUBLE_AMP,
  DOUBLE_PIPE,
  DOT,
  DOUBLE_DOT,
  TRIPLE_DOT,

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
  FAT_ARROW,

  // Keywords
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

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

#endif