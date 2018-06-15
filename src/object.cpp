#include "object.h"

/* Integer */

obj::Integer::Integer(int64_t value) : value(value) {}
std::string obj::Integer::inspect() { return std::to_string(this->value); }
obj::obj_type obj::Integer::_type() { return obj::INTEGER; }

/* Boolean */

obj::Boolean::Boolean(bool value) : value(value) {}
std::string obj::Boolean::inspect() {
  if (this->value) {
    return "true";
  } else {
    return "false";
  }
}
obj::obj_type obj::Boolean::_type() { return obj::BOOLEAN; }

/* Return Wrapper */

obj::ReturnVal::ReturnVal(Object *o) : value(o) {}
std::string obj::ReturnVal::inspect() { return this->value->inspect(); }
obj::obj_type obj::ReturnVal::_type() { return obj::RETURN_VAL; }

/* Error Value */

obj::Error::Error(std::string err) : err(err) {}
std::string obj::Error::inspect() { return this->err; }
obj::obj_type obj::Error::_type() { return obj::ERROR; }

/* Option Value */

obj::Option::Option() : value(nullptr){};
obj::Option::Option(obj_ptr value) : value(value){};
std::string obj::Option::inspect() {
  if (this->value == nullptr) {
    return "NONE";
  } else {
    // The printed syntax is not to resemble the input code, but to make it
    // clear during inspection that the value is wrapped by a contruct, so any
    // expression with the value must do so only by interacting with the
    // construct. That is the purpose of the optional.
    return "?(" + this->value->inspect() + ")";
  }
}
obj::obj_type obj::Option::_type() { return obj::OPTION; }