#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>

namespace obj {

enum obj_type {
  // For some reason, this comment keeps my autolinter happy. Meta...
  INTEGER = 0,
  BOOLEAN,
  STRING,
  RETURN_VAL,
  ERROR,
  OPTION
};

class Object;
class Bool;
class Integer;
class String;
class ReturnVal;
class Error;
class Option;

typedef std::shared_ptr<Object> obj_ptr;
typedef std::shared_ptr<Bool> bool_ptr;
typedef std::shared_ptr<Integer> int_ptr;
typedef std::shared_ptr<String> str_ptr;
typedef std::shared_ptr<ReturnVal> return_ptr;
typedef std::shared_ptr<Error> err_ptr;
typedef std::shared_ptr<Option> opt_ptr;

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

// IMPORTANT! These should only be created once each for either bool value
// to maintain two global singletons throughout evaluation.
class Bool : public Object {
 public:
  Bool(bool value);
  const bool value;

  std::string inspect();
  obj_type _type();
};

class String : public Object {
 public:
  String(std::string value);
  const std::string value;

  std::string inspect();
  obj_type _type();
};

class ReturnVal : public Object {
 public:
  ReturnVal(obj_ptr o);
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

class Option : public Object {
 public:
  Option();
  Option(obj_ptr);
  obj_ptr value;

  std::string inspect();
  obj_type _type();
};

}  // namespace obj

#endif