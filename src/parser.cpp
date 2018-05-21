#include "parser.h"

Parser::Parser(Lexer* lexer) : lexer(lexer) {
  // Set precedences

  // Register Prefix Parsing functions here

  // Register Infix Parsing functions here
}

void Parser::register_prefix(TokenType tt, PPF ppf) {
  prefix_parsers.emplace(int(tt), ppf);
}

void Parser::register_infix(TokenType tt, IPF ipf) {
  infix_parsers.emplace(int(tt), ipf);
}
