#ifndef BUILTIN_H
#define BUILTIN_H

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include "object.h"
#include "parth_error.h"

typedef obj::obj_ptr (*BI)(obj::obj_list);
typedef std::unordered_map<std::string, BI> builtin_map;

class Builtins {
 public:
  static bool is_builtin(std::string);
  static BI get_builtin(std::string);

 private:
  static builtin_map all_builtins;
};

obj::obj_ptr len(obj::obj_list);
obj::obj_ptr print(obj::obj_list);
obj::obj_ptr hash(obj::obj_list);

// Also including the global constant objects here
const obj::bool_ptr TRUE_OBJ = obj::bool_ptr(new obj::Bool(true));
const obj::bool_ptr FALSE_OBJ = obj::bool_ptr(new obj::Bool(false));
const obj::opt_ptr NONE_OBJ = obj::opt_ptr(new obj::Option());

#endif