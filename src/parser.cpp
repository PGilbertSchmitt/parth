#include "parser.h"

Parser::Parser(Lexer* lexer) : lexer(lexer) {
  // Set precedences
  precedences[TokenType::ASSIGN] = rank::ASSIGN;

  precedences[TokenType::EQ] = rank::EQUALS;
  precedences[TokenType::NEQ] = rank::EQUALS;

  precedences[TokenType::LT] = rank::COMPARE;
  precedences[TokenType::GT] = rank::COMPARE;
  precedences[TokenType::LTEQ] = rank::COMPARE;
  precedences[TokenType::GTEQ] = rank::COMPARE;

  precedences[TokenType::PLUS] = rank::SUM;
  precedences[TokenType::MINUS] = rank::SUM;

  precedences[TokenType::ASTERISK] = rank::PRODUCT;
  precedences[TokenType::SLASH] = rank::PRODUCT;
  precedences[TokenType::MODULO] = rank::PRODUCT;

  precedences[TokenType::LPAREN] = rank::CALL;
  precedences[TokenType::LBRACKET] = rank::INDEX;

  // Register Prefix Parsing functions here

  // Register Infix Parsing functions here
}

void Parser::register_prefix(TokenType tt, PPF ppf) {
  prefix_parsers.emplace(int(tt), ppf);
}

void Parser::register_infix(TokenType tt, IPF ipf) {
  infix_parsers.emplace(int(tt), ipf);
}
