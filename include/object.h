#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>

namespace obj {

enum obj_type {
  INTEGER = 0,
  BOOLEAN,
  // STRING,
  RETURN_VAL,
  ERROR,
  // OPTION
};

class Object;
class Boolean;
class Integer;
class ReturnVal;
class Error;

typedef std::shared_ptr<Object> obj_ptr;
typedef std::shared_ptr<Boolean> bool_ptr;
typedef std::shared_ptr<Integer> int_ptr;
typedef std::shared_ptr<ReturnVal> return_ptr;
typedef std::shared_ptr<Error> err_ptr;

class Object {
 public:
  virtual std::string inspect() = 0;
  virtual obj_type _type() = 0;
};

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
  obj_ptr value;

  std::string inspect();
  obj_type _type();
};

class Error : public Object {
 public:
  Error(std::string err);
  std::string err;

  std::string inspect();
  obj_type _type();
};

}  // namespace obj

#endif