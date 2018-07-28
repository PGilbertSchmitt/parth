#include "parser.h"

// Defining static rank mappings
rank_map Parser::precedences{
    {TokenType::ASSIGN, rank::ASSIGN},     {TokenType::DOUBLE_AMP, rank::LOGIC},
    {TokenType::DOUBLE_PIPE, rank::LOGIC}, {TokenType::AMP, rank::BITWISE},
    {TokenType::PIPE, rank::BITWISE},      {TokenType::CARET, rank::BITWISE},
    {TokenType::EQ, rank::EQUALS},         {TokenType::NEQ, rank::EQUALS},
    {TokenType::LT, rank::COMPARE},        {TokenType::GT, rank::COMPARE},
    {TokenType::LTEQ, rank::COMPARE},      {TokenType::GTEQ, rank::COMPARE},
    {TokenType::PLUS, rank::SUM},          {TokenType::MINUS, rank::SUM},
    {TokenType::ASTERISK, rank::PRODUCT},  {TokenType::SLASH, rank::PRODUCT},
    {TokenType::MODULO, rank::PRODUCT},    {TokenType::LPAREN, rank::CALL},
    {TokenType::LBRACKET, rank::INDEX}};

Parser::Parser(Lexer* lexer) : lexer(lexer) {
  // Register Prefix Parsing functions here
  register_prefix(TokenType::IDENT, &parse_identifier);
  // currently, parse_option should not be used outside of let expressions
  // register_prefix(TokenType::OPTION, &parse_option);
  register_prefix(TokenType::INT, &parse_integer);
  register_prefix(TokenType::LET, &parse_let);
  register_prefix(TokenType::RETURN, &parse_return);
  register_prefix(TokenType::MINUS, &parse_prefix);
  register_prefix(TokenType::BANG, &parse_prefix);
  register_prefix(TokenType::TRUE_VAL, &parse_bool);
  register_prefix(TokenType::FALSE_VAL, &parse_bool);
  register_prefix(TokenType::STRING, &parse_string);
  register_prefix(TokenType::LBRACKET, &parse_list_literal);
  register_prefix(TokenType::LPAREN, &parse_lparen);
  register_prefix(TokenType::IF, &parse_if_else);

  // Register Infix Parsing functions here
  register_infix(TokenType::ASSIGN, &parse_infix);
  register_infix(TokenType::PLUS, &parse_infix);
  register_infix(TokenType::MINUS, &parse_infix);
  register_infix(TokenType::ASTERISK, &parse_infix);
  register_infix(TokenType::SLASH, &parse_infix);
  register_infix(TokenType::MODULO, &parse_infix);
  register_infix(TokenType::AMP, &parse_infix);
  register_infix(TokenType::PIPE, &parse_infix);
  register_infix(TokenType::CARET, &parse_infix);
  register_infix(TokenType::DOUBLE_AMP, &parse_infix);
  register_infix(TokenType::DOUBLE_PIPE, &parse_infix);
  register_infix(TokenType::EQ, &parse_infix);
  register_infix(TokenType::NEQ, &parse_infix);
  register_infix(TokenType::LT, &parse_infix);
  register_infix(TokenType::GT, &parse_infix);
  register_infix(TokenType::LTEQ, &parse_infix);
  register_infix(TokenType::GTEQ, &parse_infix);

  this->next_token();
  this->next_token();
}

void Parser::register_prefix(TokenType tt, PPF ppf) {
  prefix_parsers.emplace(int(tt), ppf);
}

void Parser::register_infix(TokenType tt, IPF ipf) {
  infix_parsers.emplace(int(tt), ipf);
}

Token Parser::enqueue_next_token() {
  Token queued_token = lexer->next_token();
  unprocessed_tokens.push(queued_token);
  return queued_token;
}

Token Parser::pop_queued_token() {
  // Won't be called if empty, so no worries about checking
  Token next_tok = unprocessed_tokens.front();
  unprocessed_tokens.pop();
  return next_tok;
}

/* Accessors and checkers for parsing functions */

Token Parser::get_cur_token() { return cur_token; }

Token Parser::get_peek_token() { return peek_token; }

bool Parser::cur_token_is(TokenType tt) { return tt == cur_token.get_type(); }

bool Parser::peek_token_is(TokenType tt) { return tt == peek_token.get_type(); }

void Parser::expect_peek(TokenType tt) {
  if (peek_token_is(tt)) {
    next_token();
  } else {
    throw UnexpectedException(tt, peek_token);
  }
}

void Parser::expect_end() {
  if (peek_token_is(TokenType::NEWLINE) || peek_token_is(TokenType::EOF_VAL) ||
      peek_token_is(TokenType::RBRACE)) {
    next_token();
  } else {
    throw UnexpectedException(TokenType::EOF_VAL, peek_token);
  }
}

void Parser::eat_newlines() {
  while (cur_token_is(TokenType::NEWLINE)) {
    next_token();
  }
}

// Eats lexer tokens, but stores them in a queue so that we eventually process
// them. This allows arbitrary lookahead without having to drastically change
// how we read tokens. Currently only searching for what follows a closed paren
// (must keep track of recursion level)
Token Parser::after_paren() {
  Token read_token = peek_token;
  if (read_token.get_type() == TokenType::RPAREN) {
    return enqueue_next_token();
  }

  int paren_level = 1;
  while (!(paren_level == 0 && read_token.get_type() == TokenType::RPAREN)) {
    read_token = enqueue_next_token();

    // Making sure we're in the correct paren level
    if (read_token.get_type() == TokenType::LPAREN) {
      paren_level++;
    }
    if (read_token.get_type() == TokenType::RPAREN) {
      paren_level--;
    }
  }

  return enqueue_next_token();
}

void Parser::next_token() {
  cur_token = peek_token;
  if (unprocessed_tokens.empty()) {
    peek_token = lexer->next_token();
  } else {
    peek_token = pop_queued_token();
  }
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

  int i = 0;
  while (cur_token.get_type() != TokenType::EOF_VAL) {
    ast::node_ptr new_node = this->parse_line();
    block->push_node(new_node);
    i++;
  }

  return block;
}

ast::node_ptr Parser::parse_line() {
  this->eat_newlines();
  ast::node_ptr line = this->parse_expression(rank::LOWEST);
  this->expect_end();
  this->eat_newlines();

  return line;
}

ast::node_ptr Parser::parse_expression(rank r) {
  TokenType cur_type = this->cur_token.get_type();
  prefix_map::iterator prefix = this->prefix_parsers.find(int(cur_type));
  if (prefix == this->prefix_parsers.end()) {
    // TODO: Add prefix parsing error
    std::cout << "NO SUCH PREFIX PARSER FOR "
              << token_type_string(cur_token.get_type()) << std::endl;
    std::cout << "Current token lit is " << cur_token.get_literal()
              << std::endl;
    return ast::node_ptr();
  }
  ast::node_ptr left_expr = prefix->second(*this);

  while (!this->peek_token_is(TokenType::NEWLINE) &&
         (r < this->peek_precedence())) {
    TokenType peek_type = this->peek_token.get_type();
    infix_map::iterator infix = this->infix_parsers.find(int(peek_type));
    if (infix == this->infix_parsers.end()) {
      return left_expr;
    }

    this->next_token();
    left_expr = infix->second(*this, left_expr);
  }

  return left_expr;
}

/**********************/
/*** Prefix Parsers ***/
/**********************/

ast::node_ptr parse_identifier(Parser& p) {
  Token cur = p.get_cur_token();
  return ast::ident_ptr(new ast::Identifier(cur, cur.get_literal()));
}

ast::node_ptr parse_integer(Parser& p) {
  Token cur = p.get_cur_token();
  int64_t val = std::stoi(cur.get_literal());
  return ast::int_ptr(new ast::Integer(cur, val));
}

ast::node_ptr parse_let(Parser& p) {
  Token let_tok = p.get_cur_token();
  p.next_token();
  ast::ident_ptr name;
  if (p.cur_token_is(TokenType::IDENT)) {
    name = std::dynamic_pointer_cast<ast::Identifier>(parse_identifier(p));
  } else if (p.cur_token_is(TokenType::OPTION)) {
    name = std::dynamic_pointer_cast<ast::Option>(parse_option(p));
  } else {
    throw UnexpectedException(TokenType::IDENT, p.get_peek_token());
  }

  if (!p.peek_token_is(TokenType::ASSIGN) && name->_type() == ast::OPTION) {
    return ast::let_ptr(new ast::Let(let_tok, name));
  }
  p.expect_peek(TokenType::ASSIGN);
  p.next_token();
  ast::node_ptr right_expr = p.parse_expression(Parser::LOWEST);
  return ast::let_ptr(new ast::Let(let_tok, name, right_expr));
}

ast::node_ptr parse_return(Parser& p) {
  Token ret_tok = p.get_cur_token();
  p.next_token();
  ast::node_ptr expr = p.parse_expression(Parser::LOWEST);
  return ast::return_ptr(new ast::Return(ret_tok, expr));
}

ast::node_ptr parse_prefix(Parser& p) {
  Token pre_tok = p.get_cur_token();
  Token op = pre_tok;
  p.next_token();
  ast::node_ptr right_expr = p.parse_expression(Parser::PREFIX);
  return ast::prefix_ptr(new ast::Prefix(pre_tok, op, right_expr));
}

ast::node_ptr parse_bool(Parser& p) {
  Token bool_tok = p.get_cur_token();
  bool val = bool_tok.get_literal() == "true";
  return ast::bool_ptr(new ast::Bool(bool_tok, val));
}

ast::node_ptr parse_option(Parser& p) {
  Token cur = p.get_cur_token();
  return ast::opt_ptr(new ast::Option(cur, cur.get_literal()));
}

ast::node_ptr parse_string(Parser& p) {
  Token cur = p.get_cur_token();
  return ast::str_ptr(new ast::String(cur, cur.get_literal()));
}

ast::node_ptr parse_list_literal(Parser& p) {
  Token cur = p.get_cur_token();
  std::vector<ast::node_ptr> values =
      parse_expression_list(p, TokenType::RBRACKET);
  return ast::arr_ptr(new ast::List(cur, values));
}

ast::node_ptr parse_if_else(Parser& p) {
  Token if_tok = p.get_cur_token();
  ast::ifelse_ptr if_else = ast::ifelse_ptr(new ast::IfElse(if_tok));
  p.next_token();

  ast::node_ptr first_condition = parse_group(p);
  p.next_token();
  ast::block_ptr first_block = parse_block(p);
  // p.next_token();
  if_else->push_condition_set(first_condition, first_block);

  while (p.peek_token_is(TokenType::ELSE)) {
    p.next_token();
    p.next_token();
    ast::node_ptr condition = NULL;
    if (p.cur_token_is(TokenType::IF)) {
      p.next_token();
      condition = parse_group(p);
      p.next_token();
    }
    ast::block_ptr consequence = parse_block(p);
    if_else->push_condition_set(condition, consequence);
  }

  return if_else;
}

// This can be either a grouped expression or a function literal, so we need to
// look after the closing paren to check which it is before using the correct
// parser
ast::node_ptr parse_lparen(Parser& p) {
  Token after_close = p.after_paren();
  if (after_close.get_type() == TokenType::FAT_ARROW) {
    return parse_function_literal(p);
  } else {
    return parse_group(p);
  }
}

/*********************/
/*** Infix Parsers ***/
/*********************/

ast::node_ptr parse_infix(Parser& p, ast::node_ptr left_expr) {
  Token tok = p.get_cur_token();
  Token op = tok;
  Parser::rank prec = p.cur_precedence();
  p.next_token();
  ast::node_ptr right_expr = p.parse_expression(prec);
  return ast::infix_ptr(new ast::Infix(tok, op, left_expr, right_expr));
}

/********************************/
/*** Other parsing functions: ***/
/********************************/
/* If it's not a parsing function registered by the parsing class, then it can
 * be a more specialized parser. */

// The block parser isn't found just anywhere in the code. Since it's not a
// full expression that could be assigned to a variable, it won't be a prefix
// parser (and the curly brace syntax would break it anyways when hashes are
// added). Instead, it's only used by if-elses (and soon, switches), so those
// parsers call this directly. This could be abstracted away with
// ::parse_program()
ast::block_ptr parse_block(Parser& p) {
  Token block_tok = p.get_cur_token();
  ast::block_ptr block = ast::block_ptr(new ast::Block(block_tok));
  p.next_token();
  p.eat_newlines();
  // We don't check for a first line; empty blocks are allowed
  while (!p.cur_token_is(TokenType::RBRACE)) {
    ast::node_ptr line = p.parse_line();
    // p.next_token();
    // p.expect_block_line_end();
    block->push_node(line);
  }
  return block;
}

std::vector<ast::node_ptr> parse_expression_list(Parser& p,
                                                 TokenType end_token) {
  std::vector<ast::node_ptr> list;

  if (p.peek_token_is(end_token)) {
    p.next_token();
    return list;
  }

  p.next_token();
  list.push_back(p.parse_expression(Parser::LOWEST));

  while (p.peek_token_is(TokenType::COMMA)) {
    p.next_token();
    p.next_token();

    list.push_back(p.parse_expression(Parser::LOWEST));
  }

  p.expect_peek(end_token);

  return list;
}

ast::param_list parse_function_parameters(Parser& p) {
  ast::param_list params;

  ast::ident_ptr cur_ident =
      std::dynamic_pointer_cast<ast::Identifier>(parse_identifier(p));
  params.push_back(cur_ident);

  while (!p.peek_token_is(TokenType::RPAREN)) {
    p.expect_peek(TokenType::COMMA);
    p.next_token();
    cur_ident = std::dynamic_pointer_cast<ast::Identifier>(parse_identifier(p));
    params.push_back(cur_ident);
  }

  return params;
}

ast::node_ptr parse_function_literal(Parser& p) {
  Token tok = p.get_cur_token();

  ast::param_list params;
  if (!p.peek_token_is(TokenType::RPAREN)) {
    p.next_token();
    params = parse_function_parameters(p);
  }

  p.next_token();
  p.next_token();
  p.expect_peek(TokenType::LBRACE);
  ast::block_ptr body = parse_block(p);
  return ast::func_ptr(new ast::Function(tok, params, body));
}

// There is no group node type, since a group is just an expression that has
// been wrapped in parens. It parses the sub expression with the lowest
// precedence, to shelter it from surrounding expressions
ast::node_ptr parse_group(Parser& p) {
  p.next_token();
  ast::node_ptr expr = p.parse_expression(Parser::LOWEST);
  p.expect_peek(TokenType::RPAREN);
  return expr;
}