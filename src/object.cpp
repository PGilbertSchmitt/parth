#include "object.h"
#include "environment.h"

std::string obj::type_to_string(obj::obj_type ot) {
  switch (ot) {
    case obj::INTEGER: {
      return "INTEGER";
    } break;
    case obj::BOOLEAN: {
      return "BOOLEAN";
    } break;
    case obj::STRING: {
      return "STRING";
    } break;
    case obj::LIST: {
      return "LIST";
    } break;
    case obj::MAP: {
      return "MAP";
    } break;
    case obj::RANGE: {
      return "RANGE";
    } break;
    case obj::OPTION: {
      return "OPTION";
    } break;
    case obj::FUNCTION: {
      return "FUNCTION";
    } break;
    case obj::BUILTIN: {
      return "BUILTIN";
    } break;
    case obj::RETURN_VAL: {
      return "RETURN_VAL";
    } break;
    case obj::ERROR: {
      return "ERROR";
    } break;
    default: { return "UNKNOWN"; }
  }
}

/**********/
/* Object */
/**********/

std::string obj::Object::wrap(std::string wrapper) {
  return (wrapper + "(" + this->print() + ")");
}

/***********/
/* Integer */
/***********/

obj::Integer::Integer(int64_t _value) : value(_value) {
  this->hash_cache =
      SpookyHash::Hash64(&(_value), sizeof(_value), obj::INTEGER);
}

std::string obj::Integer::print() { return std::to_string(this->value); }

std::string obj::Integer::inspect() { return wrap("INT"); }

uint64_t obj::Integer::hash() { return this->hash_cache; }

obj::obj_type obj::Integer::_type() { return obj::INTEGER; }

/********/
/* Bool */
/********/

obj::Bool::Bool(bool _value) : value(_value) {
  this->hash_cache =
      SpookyHash::Hash64(&(_value), sizeof(_value), obj::BOOLEAN);
}

std::string obj::Bool::print() {
  if (this->value) {
    return "true";
  } else {
    return "false";
  }
}

std::string obj::Bool::inspect() { return wrap("BOOL"); }

uint64_t obj::Bool::hash() { return this->hash_cache; }

obj::obj_type obj::Bool::_type() { return obj::BOOLEAN; }

/**********/
/* String */
/**********/

obj::String::String(std::string _value) : value(_value) {
  this->hash_cache =
      SpookyHash::Hash64(_value.c_str(), _value.length(), obj::STRING);
}

std::string obj::String::print() { return this->value; }

std::string obj::String::inspect() { return wrap("STR"); }

uint64_t obj::String::hash() { return this->hash_cache; }

obj::obj_type obj::String::_type() { return obj::STRING; }

/****************/
/* Option Value */
/****************/

obj::Option::Option() : value(nullptr){};

obj::Option::Option(obj_ptr value) : value(value){};

std::string obj::Option::print() {
  // For pretty printing, I just want to show the internals. The surrounding
  // option construct is more necessary for inspection.
  if (this->value == nullptr) {
    return "NONE";
  } else {
    return this->value->print();
  }
}

std::string obj::Option::inspect() {
  if (this->value == nullptr) {
    return "?()";
  } else {
    // The printed syntax is not to resemble the input code, but to make it
    // clear during inspection that the value is wrapped by a contruct, so any
    // expression with the value must do so only by interacting with the
    // construct. That is the purpose of the optional.
    return "?(" + this->value->inspect() + ")";
  }
}

uint64_t obj::Option::hash() {
  // Hashes can be modified by certain methods, so the hash can't be cached
  // without additional logic that tracks internal changes. Until then, it will
  // calculate the hash each time using the hash of its internal object.
  uint64_t internal_hash_value;
  return SpookyHash::Hash64(&internal_hash_value, sizeof(internal_hash_value),
                            obj::OPTION);
}

obj::obj_type obj::Option::_type() { return obj::OPTION; }

/********/
/* List */
/********/

obj::List::List(obj::obj_list values) : values(values){};

std::string obj::List::inspect() { return wrap("LIST"); }

std::string obj::List::print() {
  std::ostringstream oss;
  oss << "[ ";

  // std::vector<obj::obj_ptr>::const_iterator element, last;
  for (auto element = values.begin(); element != values.end(); element++) {
    oss << (*element)->print();
    if (next(element) != values.end()) {
      oss << ", ";
    }
  }

  oss << " ]";
  return oss.str();
}

// Proud of this one (assuming it works). Much like options, the hash is
// calculated everytime until I add logic to track changes to the internal list
// for caching purposes.
uint64_t obj::List::hash() {
  obj::obj_list::const_iterator element;
  // Instead of using the type value as the seed argument for each element hash,
  // I'm using it as the starting value and using the element indices as their
  // own seeds. This preserves the element order in the hash, eg. two lists with
  // the same elements but in different orders will have different hashes. This
  // is unique to Lists, as hashes and sets are order independent.
  uint64_t hash_value = SpookyHash::Hash64("LIST", 4, obj::LIST);
  uint64_t i;
  for (element = this->values.begin(), i = 0; element != this->values.end();
       element++, i++) {
    uint64_t element_hash = (*element)->hash();
    hash_value ^= SpookyHash::Hash64(&(element_hash), sizeof(element_hash), i);
  }

  return hash_value;
}

obj::obj_type obj::List::_type() { return obj::LIST; }

/*******/
/* MAP */
/*******/

obj::Map::Map(obj::obj_map pairs) : pairs(pairs) {}

std::string obj::Map::inspect() {
  std::ostringstream oss;
  oss << "MAP( ";

  obj::obj_map::const_iterator iter;
  size_t i;
  for (iter = this->pairs.begin(), i = 1; iter != this->pairs.end();
       iter++, i++) {
    oss << iter->second.first->inspect();
    oss << ": ";
    oss << iter->second.second->inspect();
    if (i < this->pairs.size()) {
      oss << ", ";
    }
  }

  oss << " )";
  return oss.str();
}

std::string obj::Map::print() {
  std::ostringstream oss;
  oss << "{ ";

  obj::obj_map::const_iterator iter;
  size_t i;
  for (iter = this->pairs.begin(), i = 1; iter != this->pairs.end();
       iter++, i++) {
    oss << iter->second.first->print();
    oss << ": ";
    oss << iter->second.second->print();
    if (i < this->pairs.size()) {
      oss << ", ";
    }
  }

  oss << " }";
  return oss.str();
}

uint64_t obj::Map::hash() {
  uint8_t zero = 0;
  uint64_t out = SpookyHash::Hash64(&zero, sizeof(zero), obj::MAP);

  // Since there's no order to a hashmap (or rather, there shouldn't be), each
  // element's seed is just the key hash, and the message is the value hash
  obj::obj_map::const_iterator iter;
  for (iter = this->pairs.begin(); iter != this->pairs.end(); iter++) {
    uint64_t key_hash = iter->first;
    obj_ptr val_obj = iter->second.second;
    uint64_t val_hash = val_obj->hash();
    uint64_t element_hash =
        SpookyHash::Hash64(&val_hash, sizeof(val_hash), key_hash);
    out ^= element_hash;
  }

  return out;
}

obj::obj_type obj::Map::_type() { return obj::MAP; }

/*********/
/* Range */
/*********/

bool obj::Range::forward() { return end->value >= start->value; }

bool obj::Range::between(int64_t x) {
  if (forward()) {
    return start->value <= x && x <= end->value;
  } else {
    return end->value <= x && x <= start->value;
  }
}

obj::Range::Range(obj::int_ptr start, obj::int_ptr end)
    : start(start), end(end) {}

std::string obj::Range::print() { return start->print() + ".." + end->print(); }

std::string obj::Range::inspect() { return wrap("RANGE"); }

// Range hash is simply the start int hash xor'd against a shifted end int hash.
uint64_t obj::Range::hash() {
  uint64_t start_hash = start->hash();
  uint64_t end_hash = end->hash();
  return start_hash ^ (end_hash << 1);
}

obj::obj_type obj::Range::_type() { return obj::RANGE; }

/************/
/* Function */
/************/

obj::Function::Function(ast::func_ptr func_node, env::env_ptr envir)
    : func_node(func_node), envir(envir) {
  // At the moment, function hashes are random, but cached and unmodifiable.
  // Until I can figure out a graceful way to hash the arguments and contents
  // (and maybe even environment) of a function object, the hash will simply be
  // tied to the identity of the function. This means that the hash of two
  // variables pointing to the same function will be equal, but the hashes of
  // two identically written functions will actually be different. This might
  // even be the better option, but I don't know enough yet to be sure.
  uint64_t rand_seed = rand();
  this->hash_cache =
      SpookyHash::Hash64(&rand_seed, sizeof(rand_seed), obj::FUNCTION);
}

// This is the ugly print that I'm worried about, but I don't feel like printing
// functions is an essential capability. For all intents and purposes, printing
// a function is equivalent to inspection
std::string obj::Function::print() { return func_node->to_string(); }

std::string obj::Function::inspect() { return wrap("FUNC"); }

uint64_t obj::Function::hash() { return this->hash_cache; }

obj::obj_type obj::Function::_type() { return obj::FUNCTION; }

/***********/
/* Builtin */
/***********/

obj::Builtin::Builtin(BI bi) : fn(bi) {}

// Not sure about printing for builtins. May need to add name member
std::string obj::Builtin::print() { return "BI"; }

std::string obj::Builtin::inspect() { return "BI"; }

uint64_t obj::Builtin::hash() { return 0; }

obj::obj_type obj::Builtin::_type() { return obj::BUILTIN; }

/******************/
/* Return Wrapper */
/******************/

obj::ReturnVal::ReturnVal(obj_ptr o) : value(o) {}

std::string obj::ReturnVal::print() { return this->value->print(); }

std::string obj::ReturnVal::inspect() {
  return "RETURN(" + this->value->inspect() + ")";
}

uint64_t obj::ReturnVal::hash() {
  // You could never get the hash from a return object theoretically since any
  // attempt to get a hash would have to be executed after the return breaks
  // free from the block.
  return 0;
}

obj::obj_type obj::ReturnVal::_type() { return obj::RETURN_VAL; }

/***************/
/* Error Value */
/***************/

obj::Error::Error(std::string err) : err(err) {}

std::string obj::Error::print() { return this->err; }

std::string obj::Error::inspect() { return wrap("ERR"); }

uint64_t obj::Error::hash() {
  // Errors don't really need a hash value, and aren't even being used yet
  return 0;
}

obj::obj_type obj::Error::_type() { return obj::ERROR; }