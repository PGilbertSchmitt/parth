#include "builtin.h"

builtin_map Builtins::all_builtins = {
    // Builtin mappings
    {"len", &len},
    {"size", &len},
    {"count", &len},
    {"print", &print},
};

bool Builtins::is_builtin(std::string name) {
  return Builtins::all_builtins.count(name) > 0;
}

BI Builtins::get_builtin(std::string name) {
  return Builtins::all_builtins.at(name);
}

/***********/
/*** LEN ***/
/***********/

// len() accepts one argument that can only be a list, string, map, or option.
// Returns the length of the string/array as an integer object, or number of
// key/value pairs in the map, or 1/0 for a filled/none option respectively.
// Alias: size, count

obj::obj_ptr len(obj::obj_list args) {
  if (args.size() != 1) {
    throw InvalidArgsException("len(): Expected 1 argument, got " +
                               args.size());
  }

  obj::obj_ptr arg = args[0];
  uint64_t output = 0;
  switch (arg->_type()) {
    case obj::LIST: {
      obj::arr_ptr list = std::dynamic_pointer_cast<obj::List>(arg);
      output = list->values.size();
    } break;
    case obj::STRING: {
      obj::str_ptr str = std::dynamic_pointer_cast<obj::String>(arg);
      output = str->value.size();
    } break;
    case obj::MAP: {
      obj::map_ptr map = std::dynamic_pointer_cast<obj::Map>(arg);
      output = map->pairs.size();
    } break;
    case obj::OPTION: {
      obj::opt_ptr opt = std::dynamic_pointer_cast<obj::Option>(arg);
      output = (int)(opt != NONE_OBJ);
    }
    default: {
      throw InvalidArgsException("len(): Invalid argument type " +
                                 obj::type_to_string(arg->_type()) +
                                 " for builtin 'len'.");
    }
  }
  return obj::obj_ptr(new obj::Integer(output));
}

/*************/
/*** PRINT ***/
/*************/

// print() outputs a string made of every passed string object, joining first
// with spaces. Returns the full constructed string as a string object;

obj::obj_ptr print(obj::obj_list args) {
  if (args.size() == 0) {
    throw InvalidArgsException("print(): Expected more arguments than 0.");
  }

  std::ostringstream oss;
  for (auto arg = args.begin(); arg != args.end(); arg++) {
    oss << (*arg)->inspect();
    if (next(arg) != args.end()) {
      oss << " ";
    }
  }

  std::string str = oss.str();
  std::cout << str << "\n";
  return obj::str_ptr(new obj::String(str));
}

/************/
/*** HASH ***/
/************/

// hash() returns the hash of the object, the same one used by maps. This hash
// is computed with SpookyHash64 and built into every object. Can only receive a
// single element.
// Returns the hash as an integer object.

obj::obj_ptr hash(obj::obj_list args) {
  if (args.size() != 1) {
    throw InvalidArgsException("hash(): Expected 1 argument, got " +
                               args.size());
  }

  return obj::int_ptr(new obj::Integer(args.at(0)->hash()));
}
