
#ifndef AST_H
#define AST_H

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
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
  STRING,
  LIST,
  MAP,
  PREFIX,
  INFIX,
  IF_ELSE,
  FUNCTION,
  CALL,
  INDEX,
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
class Bool;
class Option;
class String;
class List;
class Map;
class Prefix;
class Infix;
class IfElse;
class Function;
class Call;
class Index;

typedef std::shared_ptr<Node> node_ptr;
typedef std::shared_ptr<Block> block_ptr;
typedef std::shared_ptr<Identifier> ident_ptr;
typedef std::shared_ptr<Let> let_ptr;
typedef std::shared_ptr<Assign> assign_ptr;
typedef std::shared_ptr<Return> return_ptr;
typedef std::shared_ptr<Integer> int_ptr;
typedef std::shared_ptr<Bool> bool_ptr;
typedef std::shared_ptr<Option> opt_ptr;
typedef std::shared_ptr<String> str_ptr;
typedef std::shared_ptr<List> arr_ptr;
typedef std::shared_ptr<Map> map_ptr;
typedef std::shared_ptr<Prefix> prefix_ptr;
typedef std::shared_ptr<Infix> infix_ptr;
typedef std::shared_ptr<IfElse> ifelse_ptr;
typedef std::shared_ptr<Function> func_ptr;
typedef std::shared_ptr<Call> call_ptr;
typedef std::shared_ptr<Index> index_ptr;

typedef std::vector<node_ptr> node_list;
typedef std::vector<ident_ptr> param_list;

// Key-value pairs for the Map literal node
typedef std::pair<node_ptr, node_ptr> kv_pair;
typedef std::vector<kv_pair> kv_list;

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

  node_list nodes;

  std::string to_string();
  void push_node(node_ptr node);
  node_type _type();
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
};

/* Integer Expression
 * Any integer number
 * RETURNS: an integer object */
class Integer : public Node {
 public:
  Integer(Token token, int64_t value);

  Token token;
  int64_t value;

  std::string to_string();
  node_type _type();
};

/* Bool Literal
 * A representation of true or false
 * RETURNS: a bool object (precreated singletons) */
class Bool : public Node {
 public:
  Bool(Token token, bool value);

  Token token;
  bool value;

  std::string to_string();
  node_type _type();
};

/* Option Literal
 * A representation of an optional type that either holds one internal value (in
 * the environment) or none at all, much like an Identifier, with the
 * possibility of no internal value. Since optionals are referenced by
 * identifiers after initialization, this should only ever appear in a let
 * statement.
 * NO RETURN, DOES NOT (CURRENTLY) APPEAR IN EXPRESSIONS OTHER THAN
 * LET-EXPRESSIONS */
class Option : public Identifier {
 public:
  Option(Token token, std::string name);

  Token token;
  std::string value;

  std::string to_string();
  node_type _type();
};

/* String Literal
 * A representation of a string of characters
 * RETURNS: A string object */
class String : public Node {
 public:
  String(Token token, std::string value);

  Token token;
  std::string value;

  std::string to_string();
  node_type _type();
};

/* List Literal
 * A representation of a type-independent ordered collection
 * RETURNS: A list object */
class List : public Node {
 public:
  List(Token token, node_list values);

  Token token;
  node_list values;

  std::string to_string();
  node_type _type();
};

/* Map Literal
 * A literal representation of key-value pairs which instantiate a hash-map
 * object called a Map, similar to Ruby's hashes or Javascript's plain objects.
 * An important note about my Maps is that any normal object value (obviously
 * excluding obj::ReturnVal and obj::Error) can be used as a key, since every
 * object has a hash() method which produces the Map key, which allows for deep
 * equality. It may be beneficial at some time to create my own implementation
 * rather than rely on the standard library's unordered_map, which will rehash
 * my hashes.
 * RETURNS: A Map object
 */
class Map : public Node {
 public:
  Map(Token token);

  Token token;
  kv_list key_value_pairs;

  std::string to_string();
  node_type _type();
};

/* Prefix Expression
 * An expression where a unary operator prepends the operand
 * eg. !(x), -(5)
 * RETURNS: the object containing value of the expression */
class Prefix : public Node {
 public:
  Prefix(Token token, Token op, node_ptr right);

  Token token;
  Token op;
  node_ptr right;

  std::string to_string();
  node_type _type();
};

/* Infix Expression
 * An expression where a binary operator separates two operands
 * eg. (x) + (4), (5) % (y)
 * RETURNS: an object containing the value of the expression */
class Infix : public Node {
 public:
  Infix(Token token, Token op, node_ptr left, node_ptr right);

  Token token;
  Token op;
  node_ptr left;
  node_ptr right;

  std::string to_string();
  node_type _type();
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
 * RETURNS: An Option object containing the value
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
  void push_condition_set(node_ptr condition, block_ptr consequence);
};

/* Function Literal
 * A function consists of a list of parameters and a block. A function can be
 * 'called' to be evaluated, where the parameters become idents that exist in a
 * special environment local to the function with values given by the call
 * expression. You know what a function is. Don't lie to me.
 * RETURNS: A function object */
class Function : public Node {
 public:
  Function(Token token, param_list params, block_ptr body);

  Token token;
  param_list params;
  block_ptr body;

  std::string to_string();
  node_type _type();
};

/* Call Expression
 * A call expression passes values to a function object, which become a part of
 * that function's environment for the duration of its block being evaluated.
 * You've almost certainly called a function or method before. Why else are you
 * reading source code?
 * RETURNS: the value of the evalution of the function's
 * block with the given arguments */
class Call : public Node {
 public:
  Call(Token token, node_ptr function, node_list args);

  Token token;
  node_ptr function;
  node_list args;

  std::string to_string();
  node_type _type();
};

/* Index Expression
 * An index expression is a way to pull a specific value from a list, string, or
 * hash object. For lists or strings, an integer is used to grab the elements
 * (starting at 0 of course). For hashes, any value can be used to key into it.
 * For all of these, functions can also be used as an index, and it's treated as
 * a mapping function. I'm very proud of this idea.
 * RETURNS: Depends
 * - If any is indexed with func 'x': returns mapped list/string/hash where 'x'
 * is called with each element, char, or key-val pair as arg(s)
 * - If list is indexed with int 'i': returns the 'i'th element
 * - If string is indexed with int 'i': returns the 'i'th char as a string
 * - If hash is indexed with value 'x': returns the value at key of 'x'
 * */
class Index : public Node {
 public:
  Index(Token token, node_ptr left, node_ptr index);

  Token token;
  node_ptr left;
  node_ptr index;

  std::string to_string();
  node_type _type();
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