#include "object.h"

/* Integer */

obj::Integer::Integer(int64_t value) : value(value) {}
std::string obj::Integer::inspect() { return std::to_string(this->value); }
obj::obj_type obj::Integer::_type() { return obj::INTEGER; }

/* Boolean */

obj::Boolean::Boolean(bool value) : value(value) {}
std::string obj::Boolean::inspect() { return std::to_string(this->value); }
obj::obj_type obj::Boolean::_type() { return obj::BOOLEAN; }

/* Return Value */

obj::ReturnVal::ReturnVal(Object *o) : value(o) {}
obj::ReturnVal::~ReturnVal() { delete this->value; }
std::string obj::ReturnVal::inspect() { return this->value->inspect(); }
obj::obj_type obj::ReturnVal::_type() { return obj::RETURN_VAL; }
