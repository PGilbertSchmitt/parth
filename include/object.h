#ifndef OBJECT_H
#define OBJECT_H

#include <string>

namespace obj {

typedef std::string obj_type;
const obj_type INTEGER = "INTEGER";
const obj_type BOOLEAN = "BOOLEAN";
const obj_type STRING = "STRING";
const obj_type RETURN_VAL = "RETURN_VAL";
const obj_type ERROR = "ERROR";
const obj_type OPTION = "OPTION";

class Object {
 public:
  virtual ~Object(){};
  virtual std::string inspect() = 0;
  virtual obj_type _type() = 0;
};  // namespace obj

class Integer : public Object {
 public:
  Integer(int64_t value);
  const int64_t value;

  std::string inspect();
  obj_type _type();
};

class Boolean : public Object {
 public:
  Boolean(bool value);
  const bool value;

  std::string inspect();
  obj_type _type();
};

class ReturnVal : public Object {
 public:
  ReturnVal(Object *);
  ~ReturnVal();
  Object *value;

  std::string inspect();
  obj_type _type();
};

}  // namespace obj

#endif