#include "parser.h"

// Defining static rank mappings
rank_map Parser::precedences{
    {TokenType::ASSIGN, rank::ASSIGN}, {TokenType::EQ, rank::EQUALS},
    {TokenType::NEQ, rank::EQUALS},    {TokenType::LT, rank::COMPARE},
    {TokenType::GT, rank::COMPARE},    {TokenType::LTEQ, rank::COMPARE},
    {TokenType::GTEQ, rank::COMPARE},  {TokenType::PLUS, rank::SUM},
    {TokenType::MINUS, rank::SUM},     {TokenType::ASTERISK, rank::PRODUCT},
    {TokenType::SLASH, rank::PRODUCT}, {TokenType::MODULO, rank::PRODUCT},
    {TokenType::LPAREN, rank::CALL},   {TokenType::LBRACKET, rank::INDEX}};

Parser::Parser(Lexer* lexer) : lexer(lexer) {
  // Register Prefix Parsing functions here
  register_prefix(TokenType::IDENT, &parse_identifier);

  // Register Infix Parsing functions here
}

void Parser::register_prefix(TokenType tt, PPF ppf) {
  prefix_parsers.emplace(int(tt), ppf);
}

void Parser::register_infix(TokenType tt, IPF ipf) {
  infix_parsers.emplace(int(tt), ipf);
}

/* Accessors and checkers for parsing functions */

Token Parser::get_cur_token() { return cur_token; }

Token Parser::get_peek_token() { return peek_token; }

bool Parser::cur_token_is(TokenType tt) { return tt == cur_token.get_type(); }

bool Parser::peek_token_is(TokenType tt) { return tt == peek_token.get_type(); }

bool Parser::expect_peek(TokenType tt) {
  if (peek_token_is(tt)) {
    next_token();
    return true;
  }

  // TODO: Add peek error
  return false;
}

void Parser::next_token() {
  cur_token = peek_token;
  peek_token = lexer->next_token();
}

Parser::rank Parser::cur_precedence() {
  rank_map::iterator cur_rank = Parser::precedences.find(cur_token.get_type());
  if (cur_rank == Parser::precedences.end()) {
    return rank::LOWEST;
  }
  return rank(cur_rank->second);
}

Parser::rank Parser::peek_precedence() {
  rank_map::iterator peek_rank =
      Parser::precedences.find(peek_token.get_type());
  if (peek_rank == Parser::precedences.end()) {
    return rank::LOWEST;
  }
  return rank(peek_rank->second);
}

ast::block_ptr Parser::parse_program() {
  Token block_token = Token(TokenType::NONE, "block", 0, 0);
  ast::block_ptr block = ast::block_ptr(new ast::Block(block_token));

  while (cur_token.get_type() != TokenType::EOF_VAL) {
    ast::node_ptr new_node = this->parse_line();
    block->push_node(new_node);
  }

  return block;
}

ast::node_ptr Parser::parse_line() {
  ast::node_ptr line = this->parse_expression(rank::LOWEST);

  while (this->cur_token.get_type() == TokenType::NEWLINE) {
    this->lexer->next_token();
  }

  return line;
}

ast::node_ptr Parser::parse_expression(rank r) {
  int cur_type = int(this->cur_token.get_type());
  PPF prefix = this->prefix_parsers[cur_type];
  if (prefix == NULL) {
    // Add prefix parsing error
    return ast::node_ptr();
  }
  ast::node_ptr left_expr = prefix(*this);

  while (!this->peek_token_is(TokenType::NEWLINE) &&
         (r < this->peek_precedence())) {
    // Do stuff
  }

  return left_expr;
}

/* Prefix Parsers */

ast::node_ptr parse_identifier(Parser& p) {
  Token cur = p.get_cur_token();
  return ast::ident_ptr(new ast::Identifier(cur, cur.get_literal()));
}

ast::node_ptr parse_integer(Parser& p) {
  Token cur = p.get_cur_token();
  int64_t val = std::stoi(cur.get_literal());
  return ast::int_ptr(new ast::Integer(cur, val));
}