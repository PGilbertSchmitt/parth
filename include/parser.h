#ifndef PARSER_H
#define PARSER_H

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "token_type.h"

// This parser is a Pratt parser, which is an improved recursive descent,
// top-down method of parsing a token stream using operator precedence. To be
// fair, I don't know much more than the basics about Pratt parsers, I'm just
// copying Thorsten Ball.

class Parser;
// Prefix Parsing Function
typedef ast::node_ptr (*PPF)(Parser &);
typedef std::unordered_map<int, PPF> prefix_map;
// Infix Parsing Function
typedef ast::node_ptr (*IPF)(Parser &, ast::node_ptr);
typedef std::unordered_map<int, IPF> infix_map;

typedef std::vector<std::string> error_list;

typedef std::unordered_map<TokenType, int, EnumClassHash> rank_map;

class Parser {
 public:
  Parser(Lexer *lexer);

  ast::block_ptr parse_program();
  ast::node_ptr parse_line();
  error_list get_all_errors();

  Token get_cur_token();
  Token get_peek_token();

  void next_token();
  bool cur_token_is(TokenType);
  bool peek_token_is(TokenType);
  void expect_peek(TokenType);
  void expect_end();

  enum rank {
    LOWEST,   // For restarting a precedence scope inside brackets
    ASSIGN,   // =
    EQUALS,   // ==
    COMPARE,  // > < >= <=
    SUM,      // + -
    PRODUCT,  // * / %
    PREFIX,   // !x -y
    CALL,     // my_func(x)
    INDEX     // my_arr[y]
  };
  ast::node_ptr parse_expression(rank r);

  rank cur_precedence();
  rank peek_precedence();

 private:
  Lexer *lexer;
  error_list errors;

  // Parsing functions
  prefix_map prefix_parsers;
  infix_map infix_parsers;
  void register_prefix(TokenType, PPF);
  void register_infix(TokenType, IPF);

  // Token management
  Token cur_token;
  Token peek_token;

  // Token Presedence
  static rank_map precedences;
};

// Prefix parsing functions
ast::node_ptr parse_identifier(Parser &p);
ast::node_ptr parse_integer(Parser &p);
ast::node_ptr parse_let(Parser &p);

// Infix parsing functions

// Errors
class UnexpectedException : public std::exception {
 public:
  explicit UnexpectedException(const TokenType &expected, const Token &got)
      : expected(expected), got(got){};

  TokenType expected;
  Token got;

  virtual const char *what() const throw() {
    std::ostringstream oss;
    oss << "Unexpected token \"" << got.get_literal() << "\" at line "
        << got.get_line() << ", col " << got.get_column() << "; Expected "
        << token_type_string(expected) << std::endl;
    std::cout << oss.str();
    return "";
  }
};

#endif