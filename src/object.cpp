#include "object.h"
#include "environment.h"

std::string obj::type_to_string(obj::obj_type ot) {
  switch (ot) {
    case obj::INTEGER: {
      return "INTEGER";
    }
    case obj::BOOLEAN: {
      return "BOOLEAN";
    }
    case obj::STRING: {
      return "STRING";
    }
    case obj::LIST: {
      return "LIST";
    }
    case obj::OPTION: {
      return "OPTION";
    }
    case obj::FUNCTION: {
      return "FUNCTION";
    }
    case obj::RETURN_VAL: {
      return "RETURN_VAL";
    }
    case obj::ERROR: {
      return "ERROR";
    }
    default: { return "UNKNOWN"; }
  }
}

/***********/
/* Integer */
/***********/

obj::Integer::Integer(int64_t _value) : value(_value) {
  this->hash_cache =
      SpookyHash::Hash64(&(_value), sizeof(_value), obj::INTEGER);
}

std::string obj::Integer::inspect() { return std::to_string(this->value); }

uint64_t obj::Integer::hash() { return this->hash_cache; }

obj::obj_type obj::Integer::_type() { return obj::INTEGER; }

/********/
/* Bool */
/********/

obj::Bool::Bool(bool _value) : value(_value) {
  this->hash_cache =
      SpookyHash::Hash64(&(_value), sizeof(_value), obj::BOOLEAN);
}

std::string obj::Bool::inspect() {
  if (this->value) {
    return "true";
  } else {
    return "false";
  }
}

uint64_t obj::Bool::hash() { return this->hash_cache; }

obj::obj_type obj::Bool::_type() { return obj::BOOLEAN; }

/**********/
/* String */
/**********/

obj::String::String(std::string _value) : value(_value) {
  this->hash_cache =
      SpookyHash::Hash64(_value.c_str(), _value.length(), obj::STRING);
}

std::string obj::String::inspect() { return "\"" + this->value + "\""; }

uint64_t obj::String::hash() { return this->hash_cache; }

obj::obj_type obj::String::_type() { return obj::STRING; }

/****************/
/* Option Value */
/****************/

obj::Option::Option() : value(nullptr){};

obj::Option::Option(obj_ptr value) : value(value){};

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

obj::List::List(std::vector<obj::obj_ptr> values) : values(values){};

std::string obj::List::inspect() {
  std::string out = "[";

  std::vector<obj::obj_ptr>::const_iterator element, last;
  last = --(values.cend());
  for (element = values.begin(); element != values.end(); element++) {
    out += (*element)->inspect();
    if (element != last) {
      out += ", ";
    }
  }

  return out + "]";
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

std::string obj::Function::inspect() { return func_node->to_string(); }

uint64_t obj::Function::hash() { return this->hash_cache; }

obj::obj_type obj::Function::_type() { return obj::FUNCTION; }

/******************/
/* Return Wrapper */
/******************/

obj::ReturnVal::ReturnVal(obj_ptr o) : value(o) {}

std::string obj::ReturnVal::inspect() {
  return "return(" + this->value->inspect() + ")";
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

std::string obj::Error::inspect() { return this->err; }

uint64_t obj::Error::hash() {
  // Errors don't really need a hash value, and aren't even being used yet
  return 0;
}

obj::obj_type obj::Error::_type() { return obj::ERROR; }