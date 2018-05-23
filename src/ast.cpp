#include "ast.h"

ast::NullNodeException nullNodeExc = ast::NullNodeException();
ast::EmptyConditionListException emptyCondExc =
    ast::EmptyConditionListException();

/*************/
/*** Block ***/
/*************/

ast::Block::Block(Token token) { this->token = token; }

void ast::Block::push_node(node_ptr node) {
  if (node == NULL) {
    throw nullNodeExc;
  }
  nodes.push_back(node);
}

std::string ast::Block::to_string() {
  std::string out;

  int nodes_size = sizeof(nodes) / sizeof(node_ptr);
  for (int i = 0; i < nodes_size; i++) {
    out += nodes[i]->to_string();
  }

  return out;
}

node_type ast::Block::type() { return node_type::BLOCK; }

/******************/
/*** Identifier **/
/******************/

ast::Identifier::Identifier(Token token, std::string value) {
  this->token = token;
  this->value = value;
}

std::string ast::Identifier::to_string() { return value; }

node_type ast::Identifier::type() { return node_type::IDENT; }

/**********************/
/*** Let Expression ***/
/**********************/

ast::Let::Let(Token token, ident_ptr name, node_ptr expression)
    : name(name), expression(expression) {
  this->token = token;
}

std::string ast::Let::to_string() {
  if (name == NULL or expression == NULL) {
    return "<! LET STATEMENT HAS NULL MEMBERS !>";
  }

  return this->token_literal() + " " + name->value + " = " +
         expression->to_string();
};

node_type ast::Let::type() { return node_type::LET; }

/*************************/
/*** Assign Expression ***/
/*************************/

ast::Assign::Assign(Token token, ident_ptr name, node_ptr expression) {
  if (name == NULL || expression == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->name = name;
  this->expression = expression;
}

std::string ast::Assign::to_string() {
  return name->to_string() + " = " + expression->to_string();
}

node_type ast::Assign::type() { return node_type::ASSIGN; }

/*************************/
/*** Return Expression ***/
/*************************/

ast::Return::Return(Token token, node_ptr expression) {
  if (expression == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->expression = expression;
}

std::string ast::Return::to_string() {
  return "return " + expression->to_string();
}

node_type ast::Return::type() { return node_type::RETURN; }

/***********************/
/*** Integer Literal ***/
/***********************/

ast::Integer::Integer(Token token, int64_t value) {
  this->token = token;
  this->value = value;
}

std::string ast::Integer::to_string() { return std::to_string(value); }

node_type ast::Integer::type() { return node_type::INTEGER; }

/***********************/
/*** Boolean Literal ***/
/***********************/

ast::Boolean::Boolean(Token token, bool value) {
  this->token = token;
  this->value = value;
}

std::string ast::Boolean::to_string() { return this->value ? "true" : "false"; }

node_type ast::Boolean::type() { return node_type::BOOLEAN; }

/*************************/
/*** Prefix Expression ***/
/*************************/

ast::Prefix::Prefix(Token token, std::string op, node_ptr right) {
  if (right == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->op = op;
  this->right = right;
}

std::string ast::Prefix::to_string() {
  return "(" + op + right->to_string() + ")";
}

node_type ast::Prefix::type() { return node_type::PREFIX; }

/************************/
/*** Infix Expression ***/
/************************/

ast::Infix::Infix(Token token, std::string op, node_ptr left, node_ptr right) {
  if (left == NULL || right == NULL) {
    throw nullNodeExc;
  }

  this->token = token;
  this->op = op;
  this->left = left;
  this->right = right;
}

std::string ast::Infix::to_string() {
  return "(" + left->to_string() + " " + op + " " + right->to_string() + ")";
}

node_type ast::Infix::type() { return node_type::INFIX; }

/*************************/
/*** IfElse Expression ***/
/*************************/

ast::IfElse::IfElse(Token token) { this->token = token; }

std::string ast::IfElse::to_string() {
  if (list.empty()) {
    throw emptyCondExc;
  }

  std::string out = "";
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

  return out;
}

node_type ast::IfElse::type() { return node_type::IF_ELSE; }

void ast::IfElse::push_condition_set(node_ptr condition,
                                     block_ptr consequence) {
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
