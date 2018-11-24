#include "builtin.h"

builtin_map Builtins::all_builtins = {
    // Builtin mappings
    {"len", &len},     {"size", &len},  {"count", &len},
    {"print", &print}, {"each", &each}, {"map", &map},
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
    } break;
    case obj::RANGE: {
      obj::range_ptr range = std::dynamic_pointer_cast<obj::Range>(arg);
      int64_t start, end;
      start = range->start->value;
      end = range->end->value;
      // Adding one since no matter what, a range is inclusive
      if (start < end) {
        output = end - start + 1;
      } else {
        output = start - end + 1;
      }
    } break;
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
    oss << (*arg)->print();
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

/************/
/*** EACH ***/
/************/

// each() accepts an iterable object type (list, string, map, range) and a
// function, and applies that function over the object. It is the primary
// looping device for parth, and used by the function-index syntactic sugar (ie.
// my_list[fn]). This is unless you prefer recursion.
//
// The function can have up to three functions depending on the context. For
// lists, strings, and ranges, each element is passed as the first arg, with the
// index being an optional second. For the map type, the first argument is the
// key, with the value for that key and the index being optional second and
// third args respectively.
//
// Returns the iterable. The returned values from the callback function are
// discarded.

obj::obj_ptr each(obj::obj_list args) { return iterate_over(args, false); }

/***********/
/*** MAP ***/
/***********/

// map() behaves exactly like each(), with the primary difference being that the
// return is a list made of the returns from each pass. There are specifics
// depending on the iterable used, but the map output is always a list.

obj::obj_ptr map(obj::obj_list args) { return iterate_over(args, true); }

/***************/
/*** HELPERS ***/
/***************/

obj::obj_ptr iterate_over(obj::obj_list args, bool is_map) {
  std::string name_of_this = is_map ? "Map" : "Each";

  if (args.size() != 2) {
    throw InvalidArgsException(
        name_of_this + " requires one iterable and one function or builtin");
  }

  obj::obj_ptr callback = args[1];
  if (callback->_type() != obj::FUNCTION && callback->_type() != obj::BUILTIN) {
    throw InvalidArgsException(
        "Callback must be a function or builtin, received " +
        obj::type_to_string(callback->_type()));
  }

  obj::obj_ptr iterable = args[0];
  obj::obj_list mapped_values;
  switch (iterable->_type()) {
    case obj::LIST: {
      auto list_obj = std::dynamic_pointer_cast<obj::List>(iterable);
      iterate_list(list_obj, callback, mapped_values, is_map);
    } break;
    case obj::STRING: {
      auto str_obj = std::dynamic_pointer_cast<obj::String>(iterable);
      iterate_string(str_obj, callback, mapped_values, is_map);
    } break;
    case obj::RANGE: {
      auto range_ptr = std::dynamic_pointer_cast<obj::Range>(iterable);
      iterate_range(range_ptr, callback, mapped_values, is_map);
    } break;
    case obj::MAP: {
      auto map_ptr = std::dynamic_pointer_cast<obj::Map>(iterable);
      iterate_map(map_ptr, callback, mapped_values, is_map);
    } break;
    default:
      throw InvalidArgsException("Each cannot accept a(n) " +
                                 obj::type_to_string(iterable->_type()) +
                                 "as its target");
  }

  // Since anything that wouldn't be caught by the switch would throw before
  // this point, I'm confident in returning without further validation
  if (is_map) {
    return obj::arr_ptr(new obj::List(mapped_values));
  }
  return iterable;
}

obj::obj_ptr iterate_list(obj::arr_ptr target, obj::obj_ptr callable,
                          obj::obj_list& keep_list, bool keep) {
  int64_t index = 0;
  for (auto element = target->values.begin(); element != target->values.end();
       element++, index++) {
    // Callback arguments
    obj::int_ptr index_arg = obj::int_ptr(new obj::Integer(index));
    obj::obj_list new_args{(*element), index_arg};

    // Run callback
    obj::obj_ptr val = applyFunction(callable, new_args);
    if (keep) {
      keep_list.push_back(val);
    }
  }

  return target;
}

obj::obj_ptr iterate_string(obj::str_ptr target, obj::obj_ptr callable,
                            obj::obj_list& keep_list, bool keep) {
  int64_t index = 0;
  for (auto ch = target->value.begin(); ch != target->value.end();
       ch++, index++) {
    // Callback arguments
    std::string cur_char = std::string(1, *ch);
    obj::str_ptr char_arg = obj::str_ptr(new obj::String(cur_char));
    obj::int_ptr index_arg = obj::int_ptr(new obj::Integer(index));
    obj::obj_list new_args{char_arg, index_arg};

    // Run callback
    obj::obj_ptr val = applyFunction(callable, new_args);
    if (keep) {
      keep_list.push_back(val);
    }
  }

  return target;
}

obj::obj_ptr iterate_range(obj::range_ptr target, obj::obj_ptr callable,
                           obj::obj_list& keep_list, bool keep) {
  int64_t index = 0;
  int64_t iter = target->start->value;
  int64_t end = target->end->value;
  bool forward = target->forward();
  int mod = forward ? 1 : -1;
  while (true) {
    // Callback arguments
    obj::int_ptr iter_arg = obj::int_ptr(new obj::Integer(iter));
    obj::int_ptr index_arg = obj::int_ptr(new obj::Integer(index));
    obj::obj_list new_args{iter_arg, index_arg};

    // Run callback
    obj::obj_ptr val = applyFunction(callable, new_args);
    if (keep) {
      keep_list.push_back(val);
    }

    index++;
    iter += mod;
    if ((forward && iter > end) || (!forward && iter < end)) {
      break;
    }
  }

  return target;
}

// NOTE: Although an index is maintained while iterating over a map, it should
// be understood that there is no guarantee of ordering in a map, so the index
// doesn't mean anything significant or correlate to the either key or value,
// except to count how many loops have occurred. It can be useful in context,
// but does not inherently signify anything.
obj::obj_ptr iterate_map(obj::map_ptr target, obj::obj_ptr callable,
                         obj::obj_list& keep_list, bool keep) {
  int64_t index = 0;
  for (auto iter = target->pairs.begin(); iter != target->pairs.end();
       iter++, index++) {
    // Callback arguments
    obj::obj_pair kv_pair = iter->second;
    obj::obj_ptr key_arg = kv_pair.first;
    obj::obj_ptr val_arg = kv_pair.second;
    obj::int_ptr index_arg = obj::int_ptr(new obj::Integer(index));
    obj::obj_list new_args{key_arg, val_arg, index_arg};

    // Run callback
    obj::obj_ptr val = applyFunction(callable, new_args);
    if (keep) {
      keep_list.push_back(val);
    }
  }

  return target;
}