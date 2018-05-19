
#pragma once
#include <exception>
#include <string>
#include <vector>
#include "token.h"

namespace Ast {

/* Node:
 * Every node is an expression, meaning every expression can be stored and
 * interpreted as another node. A node will always evaluate to a value */
class Node {
 public:
  Token token;
  std::string token_literal() { return token.getLiteral(); };
  virtual std::string to_string() = 0;
};

/* Block Expression
 * A list of nodes
 * RETURNS: the value of the last expression in the nodes vector
 * NOTE: this is the only node as of yet that doesn't accept all of its members
 * in the constructor, and instead builds the internals afterwards. There was no
 * reason to do it this way. */
class Block : public Node {
 public:
  Block(Token token);
  ~Block();

  std::vector<Node *> nodes;

  std::string token_literal();
  std::string to_string();

  void push_node(Node *node);
};

/* Identifier:
 * Holds the name of anything that needs one (variables, builtins, etc)
 * RETURNS: the value in the environment at the key of the identifier's
 * value */
class Identifier : public Node {
 public:
  Identifier(Token token, std::string value);

  Token token;
  std::string value;

  std::string token_literal();
  std::string to_string();
};

/* Let Expression
 * Declares and instantiates variables
 * RETURNS: the value returned by the internal expression */
class Let : public Node {
 public:
  Let(Token token, Identifier *name, Node *expression);
  ~Let();

  Token token;
  const Identifier *name;
  Node *expression;

  std::string token_literal();
  std::string to_string();
};

/* Assign Expression
 * Allows variables that have already been declared to be given new values
 * RETURNS: the value returned by the internal expression */
class Assign : public Node {
 public:
  Assign(Token token, Identifier *name, Node *expression);
  ~Assign();

  Token token;
  Identifier *name;
  Node *expression;

  std::string token_literal();
  std::string to_string();
};

/* Return Expression
 * Pulls execution out of a block, bringing with it the value of the internal
 * expression. Mostly useful for early returns since blocks automatically return
 * the value of the final expression
 * RETURNS: the value returned by the internal expression */
class Return : public Node {
 public:
  Return(Token token, Node *expression);
  ~Return();

  Token token;
  Node *expression;

  std::string token_literal();
  std::string to_string();
};

/* Integer Expression
 * Any integer number
 * RETURNS: an integer */
class Integer : public Node {
 public:
  Integer(Token token, int64_t value);

  Token token;
  int64_t value;

  std::string token_literal();
  std::string to_string();
};

/* Boolean Literal
 * A representation of true or false
 * RETURNS: the boolean value of the expression */
class Boolean : public Node {
 public:
  Boolean(Token token, bool value);

  Token token;
  bool value;

  std::string token_literal();
  std::string to_string();
};

/* Prefix Expression
 * An expression where a unary operator prepends the operand
 * eg. !(x), -(5)
 * RETURNS: the value of the expression */
class Prefix : public Node {
 public:
  Prefix(Token token, std::string op, Node *right);
  ~Prefix();

  Token token;
  std::string op;
  Node *right;

  std::string token_literal();
  std::string to_string();
};

/* Infix Expression
 * An expression where a binary operator separates two operands
 * eg. (x) + (4), (5) % (y)
 * RETURNS: the value of the expression */
class Infix : public Node {
 public:
  Infix(Token token, std::string op, Node *left, Node *right);
  ~Infix();

  Token token;
  Node *left;
  Node *right;
  std::string op;

  std::string token_literal();
  std::string to_string();
};

/* IfElse Expression
 * A branching expression with at least one conditional statement and block, but
 * can have indefinitely many if {} else if {} else if...
 * RETURNS: An Option containing the value
 * returned by the block expression paired with the first conditional that
 * returns true. If there is no
 * default and none of the conditions return true, a None option is returned. A
 * None is also returned if the first truthy condition is paired with an empty
 * block.
 * NOTE: An "else" without an immediate "if" is given an emtpy conditional that
 * always will evaluate to true, so it will always execute its block.
 * NOTE: This node does not guarantee that there will be at least one
 * condition_set in the list, so the parser will have to ensure that itself. An
 * error is thrown on the to_string method if this is the case. */
class IfElse : public Node {
 public:
  IfElse(Token token);
  ~IfElse();

  Token token;
  std::vector<condition_set> list;

  std::string token_literal();
  std::string to_string();

  void push_condition_set(Node *condition, Block *consequence);
};

/* A pair of a condition and the consequence should that condition be evaluated
 * to true */
struct condition_set {
  Node *condition;
  Block *consequence;
};

// TODO: Have these exceptions be raised with the line and column numbers from
// the tokens.

/* No NULL pointers should be accepted by any node constructor or method, so
 * there should be a check that throws this error. If there's a better way to go
 * about this, please let me know. This feels weirdly tedious. */
class NullNodeException : public std::exception {
  virtual const char *what() const throw() {
    return "NullNodeException: A null node member was passed to a "
           "node object method or constructor";
  }
} nullNodeExc;

/* The to_string method of the IfElse will throw this if called when the
 * condition_set list is empty */
class EmptyConditionListException : public std::exception {
  virtual const char *what() const throw() {
    return "EmptyConditionListException: An IfElse expression node must have "
           "at least on condition_set";
  }
} emptyCondExc;

}  // namespace Ast