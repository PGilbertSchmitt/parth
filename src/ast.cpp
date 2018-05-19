#include "ast.h"

/*************/
/*** BLOCK ***/
/*************/

Ast::Block::Block(Token token) { this->token = token; }

Ast::Block::~Block() { nodes.clear(); }

void Ast::Block::push_node(Node *node) {
  if (node == NULL) {
    throw nullNodeExc;
  }
  nodes.push_back(node);
}

std::string Ast::Block::to_string() {
  std::string out;

  int nodes_size = sizeof(nodes) / sizeof(Node *);
  for (int i = 0; i < nodes_size; i++) {
    out += nodes[i]->to_string();
  }

  return out;
}

/******************/
/*** IDENTIFIER ***/
/******************/

Ast::Identifier::Identifier(Token token, std::string value) {
  this->value = value;
}

std::string Ast::Identifier::to_string() { return value; }

/**********************/
/*** Let Expression ***/
/**********************/

Ast::Let::Let(Token token, Identifier *name, Node *expression) {
  if (expression == NULL || name == NULL) {
    throw nullNodeExc;
  }
  this->name = name;
  this->expression = expression;
  this->token = token;
}

Ast::Let::~Let() {
  delete this->name;
  delete this->expression;
}

std::string Ast::Let::to_string() {
  if (name == NULL or expression == NULL) {
    return "<! LET STATEMENT HAS NULL MEMBERS !>";
  }

  return token_literal() + " " + name->value + " = " + expression->to_string();
};

/*************************/
/*** Assign Expression ***/
/*************************/

Ast::Assign::Assign(Token token, Identifier *name, Node *expression) {
  if (name == NULL || expression == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->name = name;
  this->expression = expression;
}

Ast::Assign::~Assign() {
  delete this->name;
  delete this->expression;
}

std::string Ast::Assign::to_string() {
  return name->to_string() + " = " + expression->to_string();
}

/*************************/
/*** Return Expression ***/
/*************************/

Ast::Return::Return(Token token, Node *expression) {
  if (expression == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->expression = expression;
}

Ast::Return::~Return() { delete this->expression; }

std::string Ast::Return::to_string() {
  return "return " + expression->to_string();
}

/***********************/
/*** Integer Literal ***/
/***********************/

Ast::Integer::Integer(Token token, int64_t value) {
  this->token = token;
  this->value = value;
}

std::string Ast::Integer::to_string() { return std::to_string(value); }

/***********************/
/*** Boolean Literal ***/
/***********************/

Ast::Boolean::Boolean(Token token, bool value) {
  this->token = token;
  this->value = value;
}

std::string Ast::Boolean::to_string() { return this->value ? "true" : "false"; }

/*************************/
/*** Prefix Expression ***/
/*************************/

Ast::Prefix::Prefix(Token token, std::string op, Node *right) {
  if (right == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->op = op;
  this->right = right;
}

Ast::Prefix::~Prefix() { delete this->right; }

std::string Ast::Prefix::to_string() {
  return "(" + op + right->to_string() + ")";
}

/************************/
/*** Infix Expression ***/
/************************/

Ast::Infix::Infix(Token token, std::string op, Node *left, Node *right) {
  if (left == NULL || right == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->op = op;
  this->left = left;
  this->right = right;
}

Ast::Infix::~Infix() {
  delete this->left;
  delete this->right;
}

std::string Ast::Infix::to_string() {
  return "(" + left->to_string() + " " + op + " " + right->to_string() + ")";
}

/*************************/
/*** IfElse Expression ***/
/*************************/

Ast::IfElse::IfElse(Token token) { this->token = token; }

Ast::IfElse::~IfElse() {
  for (std::vector<condition_set>::iterator block = list.begin();
       block != list.end(); block++) {
    delete block->condition;
    delete block->consequence;
  }
}

std::string Ast::IfElse::to_string() {
  if (list.empty()) {
    throw emptyCondExc;
  }

  std::string out;
  bool first = true;

  for (std::vector<condition_set>::iterator block = list.begin();
       block != list.end(); block++) {
    if (!first) {
      out += " else ";
    }
    if (block->condition != NULL) {
      out += "if ";
      out += block->condition->to_string();
    }
    out += " ";
    out += block->consequence->to_string();

    first = false;
  }
}

void Ast::IfElse::push_condition_set(Node *condition, Block *consequence) {
  // Not checking for condition after the first since a null condition ptr
  // represents an else with no followed if. If any else-ifs follow an else, it
  // is just unreachable code and no error is raised (TODO: Consider a warning)
  if (consequence == NULL || (list.empty() && condition == NULL)) {
    throw nullNodeExc;
  }

  condition_set set;
  set.condition = condition;
  set.consequence = consequence;
  list.push_back(set);
}
