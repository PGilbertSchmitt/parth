#ifndef BUILTIN_H
#define BUILTIN_H

#include <memory>
#include <string>
#include <unordered_map>
#include "eval.h"
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
obj::obj_ptr each(obj::obj_list);
obj::obj_ptr map(obj::obj_list);

// Helpers

obj::obj_ptr iterate_over(obj::obj_list, bool);
obj::obj_ptr iterate_list(obj::arr_ptr, obj::obj_ptr, obj::obj_list&, bool);
obj::obj_ptr iterate_string(obj::str_ptr, obj::obj_ptr, obj::obj_list&, bool);
obj::obj_ptr iterate_range(obj::range_ptr, obj::obj_ptr, obj::obj_list&, bool);
obj::obj_ptr iterate_map(obj::map_ptr, obj::obj_ptr, obj::obj_list&, bool);

// Also including the global constant objects here since they are builtin values
const obj::bool_ptr TRUE_OBJ = obj::bool_ptr(new obj::Bool(true));
const obj::bool_ptr FALSE_OBJ = obj::bool_ptr(new obj::Bool(false));
const obj::opt_ptr NONE_OBJ = obj::opt_ptr(new obj::Option());

#endif