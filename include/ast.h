
#ifndef AST_H
#define AST_H

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "object.h"
#include "token.h"

namespace ast {

enum node_type {
  BLOCK = 0,
  IDENT,
  LET,
  ASSIGN,
  RETURN,
  INTEGER,
  BOOLEAN,
  OPTION,
  PREFIX,
  INFIX,
  IF_ELSE,
  VALUE
};

std::string node_type_string(node_type);

class Node;
class Block;
class Identifier;
class Let;
class Assign;
class Return;
class Integer;
class Boolean;
class Option;
class Prefix;
class Infix;
class IfElse;
class Value;

typedef std::shared_ptr<Node> node_ptr;
typedef std::shared_ptr<Block> block_ptr;
typedef std::shared_ptr<Identifier> ident_ptr;
typedef std::shared_ptr<Let> let_ptr;
typedef std::shared_ptr<Assign> assign_ptr;
typedef std::shared_ptr<Return> return_ptr;
typedef std::shared_ptr<Integer> int_ptr;
typedef std::shared_ptr<Boolean> bool_ptr;
typedef std::shared_ptr<Option> opt_ptr;
typedef std::shared_ptr<Prefix> prefix_ptr;
typedef std::shared_ptr<Infix> infix_ptr;
typedef std::shared_ptr<IfElse> ifelse_ptr;
typedef std::shared_ptr<Value> val_ptr;

/* Node:
 * Every node is an expression, meaning every expression can be stored and
 * interpreted as another node. A node will always evaluate to a value */
class Node {
 public:
  Token token;
  virtual std::string token_literal() { return token.get_literal(); };
  virtual std::string to_string() = 0;
  // The node type will be checked upon during evaluation so that the nodes can
  // be dynamically cast into their proper types. I fear this may be a messy way
  // to go about this, and I'm doing something that was okay in my previous
  // Golang code but isn't technically considered proper in C++. However, the
  // language does allow for this, and I'm confident that it will work well
  // enough for now.
  virtual node_type _type() = 0;
  virtual bool is_reducible() = 0;
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

  std::vector<node_ptr> nodes;

  std::string to_string();
  void push_node(node_ptr node);
  node_type _type();
  bool is_reducible();
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

  std::string to_string();
  node_type _type();
  bool is_reducible();

 protected:
  Identifier();
};

/* Let Expression
 * Declares and instantiates variables
 * RETURNS: the value returned by the internal expression */
class Let : public Node {
 public:
  Let(Token token, ident_ptr name);
  Let(Token token, ident_ptr name, node_ptr expression);

  Token token;
  const ident_ptr name;
  node_ptr expression;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Assign Expression
 * Allows variables that have already been declared to be given new values
 * RETURNS: the value returned by the internal expression */
class Assign : public Node {
 public:
  Assign(Token token, ident_ptr name, node_ptr expression);

  Token token;
  ident_ptr name;
  node_ptr expression;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Return Expression
 * Pulls execution out of a block, bringing with it the value of the internal
 * expression. Mostly useful for early returns since blocks automatically return
 * the value of the final expression
 * RETURNS: the value returned by the internal expression */
class Return : public Node {
 public:
  Return(Token token, node_ptr expression);

  Token token;
  node_ptr expression;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Integer Expression
 * Any integer number
 * RETURNS: an integer */
class Integer : public Node {
 public:
  Integer(Token token, int64_t value);

  Token token;
  int64_t value;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Boolean Literal
 * A representation of true or false
 * RETURNS: the boolean value of the expression */
class Boolean : public Node {
 public:
  Boolean(Token token, bool value);

  Token token;
  bool value;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Option Literal
 * A representation of an optional type that either holds one internal value (in
 * the environment) or none at all, much like an Identifier, with the
 * possibility of no internal value. Since optionals are referenced by
 * identifiers after initialization, this should only ever appear in a let
 * statement.
 * NO RETURN, DOES NOT (CURRENTLY) APPEAR IN EXPRESSIONS */

class Option : public Identifier {
 public:
  Option(Token token, std::string name);

  Token token;
  std::string value;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Prefix Expression
 * An expression where a unary operator prepends the operand
 * eg. !(x), -(5)
 * RETURNS: the value of the expression */
class Prefix : public Node {
 public:
  Prefix(Token token, Token op, node_ptr right);

  Token token;
  Token op;
  node_ptr right;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* Infix Expression
 * An expression where a binary operator separates two operands
 * eg. (x) + (4), (5) % (y)
 * RETURNS: the value of the expression */
class Infix : public Node {
 public:
  Infix(Token token, Token op, node_ptr left, node_ptr right);

  Token token;
  Token op;
  node_ptr left;
  node_ptr right;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* A pair of a condition and the consequence should that condition be evaluated
 * to true */
struct condition_set {
  node_ptr condition;
  block_ptr consequence;
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

  Token token;
  std::vector<condition_set> list;

  std::string to_string();
  node_type _type();
  bool is_reducible();
  void push_condition_set(node_ptr condition, block_ptr consequence);
};

/* Value Node
 * The value node is unique, in that it is not something that is put into the
 * AST by the parser but by the evaluator. In order to handle step-by-step
 * evaluation, I've decided to design the AST so that the tree can be reduced in
 * discrete steps. When the lowest node with irreducible children is evaluated,
 * it is replaced by a value node containing the object recording a value (as
 * opposed to a literal, which would be cumbersome to deal with as a value).
 * This adds complexity to the AST and to the evaluator, but saves me having to
 * write my own custom assembly-like intermediate language and interpreter
 * (which I have zero clue on how to do).
 */
class Value : public Node {
 public:
  Value(Token token, obj::obj_ptr object);

  Token token;
  obj::obj_ptr object;

  std::string to_string();
  node_type _type();
  bool is_reducible();
};

/* No NULL pointers should be accepted by any node constructor or method, so
 * there should be a check that throws this error. If there's a better way to go
 * about this, please let me know. This feels weirdly tedious. */
class NullNodeException : public std::exception {
 public:
  NullNodeException(std::string opt = "") { this->opt = opt; }
  std::string opt;
  virtual const char *what() const throw() {
    std::cout << opt << std::endl;
    return "NullNodeException: A null node member was passed to a node object "
           "method or constructor";
  }
};

/* The to_string method of the IfElse will throw this if called when the
 * condition_set list is empty */
class EmptyConditionListException : public std::exception {
  virtual const char *what() const throw() {
    return "EmptyConditionListException: An IfElse expression node must have "
           "at least on condition_set";
  }
};

}  // namespace ast

#endif