#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "SpookyV2.h"
#include "ast.h"

// Need to forward declare environment
namespace env {
class Environment;
typedef std::shared_ptr<Environment> env_ptr;
}  // namespace env

namespace obj {

enum obj_type {
  // For some reason, this comment keeps my autolinter happy. Meta...
  INTEGER = 0,
  BOOLEAN,
  STRING,
  LIST,
  MAP,
  OPTION,
  FUNCTION,
  RETURN_VAL,
  ERROR
};

std::string type_to_string(obj_type);

class Object;
class Bool;
class Integer;
class String;
class Option;
class List;
class Map;
class Function;
class ReturnVal;
class Error;

typedef std::shared_ptr<Object> obj_ptr;
typedef std::shared_ptr<Bool> bool_ptr;
typedef std::shared_ptr<Integer> int_ptr;
typedef std::shared_ptr<String> str_ptr;
typedef std::shared_ptr<Option> opt_ptr;
typedef std::shared_ptr<List> arr_ptr;
typedef std::shared_ptr<Map> map_ptr;
typedef std::shared_ptr<Function> func_ptr;
typedef std::shared_ptr<ReturnVal> return_ptr;
typedef std::shared_ptr<Error> err_ptr;

typedef std::vector<obj_ptr> obj_list;
// Might later look for a less convoluted way to represent the keys and values,
// but currenty, accessing the key of an obj_map looks like:
// => obj_map->second.first
// While accessing the value looks like:
// => obj_map->second.second
// I know it's gross, I'll figure something out. Maybe it's own class...
typedef std::pair<obj_ptr, obj_ptr> obj_pair;
typedef std::unordered_map<uint64_t, obj_pair> obj_map;

class Object {
 public:
  virtual std::string inspect() = 0;
  virtual uint64_t hash() = 0;
  virtual obj_type _type() = 0;
};

class Integer : public Object {
 public:
  Integer(int64_t value);
  const int64_t value;

  std::string inspect();
  uint64_t hash();
  obj_type _type();

 private:
  uint64_t hash_cache;
};

// IMPORTANT! These should only be created once each for either bool value
// to maintain two global singletons throughout evaluation.
class Bool : public Object {
 public:
  Bool(bool value);
  const bool value;

  std::string inspect();
  uint64_t hash();
  obj_type _type();

 private:
  uint64_t hash_cache;
};

class String : public Object {
 public:
  String(std::string value);
  const std::string value;

  std::string inspect();
  uint64_t hash();
  obj_type _type();

 private:
  uint64_t hash_cache;
};

class Option : public Object {
 public:
  Option();
  Option(obj_ptr);
  obj_ptr value;

  std::string inspect();
  uint64_t hash();
  obj_type _type();
};

class List : public Object {
 public:
  List(obj_list values);
  const obj_list values;

  std::string inspect();
  uint64_t hash();
  obj_type _type();
};

class Map : public Object {
 public:
  Map(obj_map pairs);
  const obj_map pairs;

  std::string inspect();
  uint64_t hash();
  obj_type _type();
};

class Function : public Object {
 public:
  Function(ast::func_ptr func_node, env::env_ptr envir);
  ast::func_ptr func_node;
  env::env_ptr envir;

  std::string inspect();
  uint64_t hash();
  obj_type _type();

 private:
  uint64_t hash_cache;
};

class ReturnVal : public Object {
 public:
  ReturnVal(obj_ptr o);
  obj_ptr value;

  std::string inspect();
  uint64_t hash();
  obj_type _type();
};

class Error : public Object {
 public:
  Error(std::string err);
  std::string err;

  std::string inspect();
  uint64_t hash();
  obj_type _type();
};

}  // namespace obj

#endif