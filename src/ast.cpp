#include "ast.h"
#include "object.h"

ast::NullNodeException nullNodeExc = ast::NullNodeException();
ast::EmptyConditionListException emptyCondExc =
    ast::EmptyConditionListException();

/*************/
/*** Block ***/
/*************/

ast::Block::Block(Token token) { this->token = token; }

void ast::Block::push_node(node_ptr node) {
  if (node == NULL) {
    throw ast::NullNodeException("In Block:");
  }
  nodes.push_back(node);
}

std::string ast::Block::to_string() {
  std::string out;

  int nodes_size = nodes.size();
  out += "{\n";
  for (int i = 0; i < nodes_size; i++) {
    out += nodes[i]->to_string();
    if (i < nodes_size - 1) {
      out += ",\n";
    }
  }
  out += "\n}";

  return out;
}

ast::node_type ast::Block::_type() { return ast::BLOCK; }

/******************/
/*** Identifier **/
/******************/

ast::Identifier::Identifier(){};

ast::Identifier::Identifier(Token token, std::string value) {
  this->token = token;
  this->value = value;
}

std::string ast::Identifier::to_string() { return "IDENT(" + value + ")"; }

ast::node_type ast::Identifier::_type() { return ast::IDENT; }

/**********************/
/*** Let Expression ***/
/**********************/

ast::Let::Let(Token token, ident_ptr name) : name(name), expression(nullptr) {
  this->token = token;
}

ast::Let::Let(Token token, ident_ptr name, node_ptr expression)
    : name(name), expression(expression) {
  this->token = token;
}

std::string ast::Let::to_string() {
  if (name == NULL) {
    return "<! LET STATEMENT HAS NULL IDENT !>";
  }

  std::string out = "let " + name->to_string();
  if (expression != NULL) {
    out += (" = " + expression->to_string());
  }

  return out;
};

ast::node_type ast::Let::_type() { return ast::LET; }

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

ast::node_type ast::Assign::_type() { return ast::ASSIGN; }

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

ast::node_type ast::Return::_type() { return ast::RETURN; }

/***********************/
/*** Integer Literal ***/
/***********************/

ast::Integer::Integer(Token token, int64_t value) {
  this->token = token;
  this->value = value;
}

std::string ast::Integer::to_string() { return std::to_string(value); }

ast::node_type ast::Integer::_type() { return ast::INTEGER; }

/***********************/
/*** Bool Literal ***/
/***********************/

ast::Bool::Bool(Token token, bool value) {
  this->token = token;
  this->value = value;
}

std::string ast::Bool::to_string() { return this->value ? "true" : "false"; }

ast::node_type ast::Bool::_type() { return ast::BOOLEAN; }

/**********************/
/*** Option Literal ***/
/**********************/

ast::Option::Option(Token token, std::string value) {
  this->token = token;
  this->value = value;
}

std::string ast::Option::to_string() { return this->value; }

ast::node_type ast::Option::_type() { return ast::OPTION; }

/**********************/
/*** String Literal ***/
/**********************/

ast::String::String(Token token, std::string value) {
  this->token = token;
  this->value = value;
}

std::string ast::String::to_string() { return "\"" + this->value + "\""; }

ast::node_type ast::String::_type() { return ast::STRING; }

/*********************/
/*** List Literal ***/
/*********************/

ast::List::List(Token token, ast::node_list values) {
  this->token = token;
  this->values = values;
}

std::string ast::List::to_string() {
  std::string out = "[";

  ast::node_list::iterator iter, last;
  last = --(values.end());
  for (iter = values.begin(); iter != values.end(); iter++) {
    out += (*iter)->to_string();
    if (iter != last) {
      out += ", ";
    }
  }

  return out + "]";
}

ast::node_type ast::List::_type() { return ast::LIST; }

/*******************/
/*** Map Literal ***/
/*******************/

ast::Map::Map(Token token) { this->token = token; }

std::string ast::Map::to_string() {
  size_t pair_size = this->key_value_pairs.size();
  if (pair_size == 0) {
    return "(MAP){}";
  }

  ast::kv_list pairs = this->key_value_pairs;
  std::string out = "(MAP){ ";

  uint i;
  ast::kv_list::iterator iter;
  for (iter = pairs.begin(), i = 1; iter != pairs.end(); iter++, i++) {
    out += iter->first->to_string();
    out += ": ";
    out += iter->second->to_string();

    if (i < pairs.size()) {
      out += ", ";
    } else {
      out += " }";
    }
  }

  return out;
}

ast::node_type ast::Map::_type() { return ast::MAP; }

void ast::Map::push_kv_pair(ast::node_ptr k, ast::node_ptr v) {
  ast::kv_pair new_kv = ast::kv_pair(k, v);
  this->key_value_pairs.push_back(new_kv);
}

/*************************/
/*** Prefix Expression ***/
/*************************/

ast::Prefix::Prefix(Token token, Token op, node_ptr right) {
  if (right == NULL) {
    throw ast::NullNodeException("In Prefix:");
    ;
  }

  this->token = token;
  this->op = op;
  this->right = right;
}

std::string ast::Prefix::to_string() {
  return "(" + op.get_literal() + right->to_string() + ")";
}

ast::node_type ast::Prefix::_type() { return ast::PREFIX; }

/************************/
/*** Infix Expression ***/
/************************/

ast::Infix::Infix(Token token, Token op, node_ptr left, node_ptr right) {
  if (left == NULL || right == NULL) {
    throw ast::NullNodeException("In Infix:");
    ;
  }

  this->token = token;
  this->op = op;
  this->left = left;
  this->right = right;
}

std::string ast::Infix::to_string() {
  return "(" + left->to_string() + " " + op.get_literal() + " " +
         right->to_string() + ")";
}

ast::node_type ast::Infix::_type() { return ast::INFIX; }

/************************/
/*** Group Expression ***/
/************************/

ast::Group::Group(Token token, ast::node_ptr expr) {
  this->token = token;
  this->expr = expr;
}

std::string ast::Group::to_string() {
  return "(" + this->expr->to_string() + ")";
}

ast::node_type ast::Group::_type() { return ast::GROUP; }

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

ast::node_type ast::IfElse::_type() { return ast::IF_ELSE; }

void ast::IfElse::push_condition_set(node_ptr condition,
                                     block_ptr consequence) {
  // Not checking for condition after the first since a null condition ptr
  // represents an else with no followed if. If any else-ifs follow an else,
  // it is just unreachable code and no error is raised (TODO: Consider a
  // warning)
  if (consequence == NULL || (list.empty() && condition == NULL)) {
    throw nullNodeExc;
  }

  condition_set set;
  set.condition = condition;
  set.consequence = consequence;
  list.push_back(set);
}

/************************/
/*** Function Literal ***/
/************************/

ast::Function::Function(Token token, ast::param_list params,
                        ast::block_ptr body) {
  this->token = token;
  this->params = params;
  this->body = body;
}

std::string ast::Function::to_string() {
  std::string out = "(";
  ast::param_list::iterator last = --(params.end());
  for (ast::param_list::iterator iter = params.begin(); iter != params.end();
       iter++) {
    out += (*iter)->to_string();
    if (iter != last) {
      out += ", ";
    }
  }
  out += ") => {\n";
  out += body->to_string();
  return out + "\n}\n";
}

ast::node_type ast::Function::_type() { return ast::FUNCTION; }

// A function literal itself is not actually reducible. When it's being
// called, it's body is reduced, but the function literal itself is good to go
// immediately.
/***********************/
/*** Call Expression ***/
/***********************/

ast::Call::Call(Token token, ast::node_ptr function, ast::node_list args) {
  this->token = token;
  this->function = function;
  this->args = args;
}

std::string ast::Call::to_string() {
  std::string out = function->to_string() + "(";

  ast::node_list::iterator last = --(args.end());
  for (ast::node_list::iterator iter = args.begin(); iter != args.end();
       iter++) {
    out += (*iter)->to_string();
    if (iter != last) {
      out += ", ";
    }
  }

  return out += ")";
}

ast::node_type ast::Call::_type() { return ast::CALL; }

/************************/
/*** Index Expression ***/
/************************/

ast::Index::Index(Token token, node_ptr left, node_ptr index) {
  this->token = token;
  this->left = left;
  this->index = index;
}

std::string ast::Index::to_string() {
  return left->to_string() + "[" + index->to_string() + "]";
}

ast::node_type ast::Index::_type() { return ast::INDEX; }

/***************/
/*** Helpers ***/
/***************/

std::string ast::node_type_string(node_type _type) {
  switch (_type) {
    case ast::BLOCK:
      return "BLOCK";
    case ast::IDENT:
      return "IDENT";
    case ast::LET:
      return "LET";
    case ast::ASSIGN:
      return "ASSIGN";
    case ast::RETURN:
      return "RETURN";
    case ast::INTEGER:
      return "INT";
    case ast::BOOLEAN:
      return "BOOLEAN";
    case ast::OPTION:
      return "OPTION";
    case ast::STRING:
      return "STRING";
    case ast::LIST:
      return "LIST";
    case ast::MAP:
      return "MAP";
    case ast::PREFIX:
      return "PREFIX";
    case ast::INFIX:
      return "INFIX";
    case ast::GROUP:
      return "GROUP";
    case ast::IF_ELSE:
      return "IF_ELSE";
    case ast::FUNCTION:
      return "FUNCTION";
    case ast::CALL:
      return "CALL";
    case ast::INDEX:
      return "INDEX";
    default:
      return "UNKNOWN";
  }
}