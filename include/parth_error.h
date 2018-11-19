#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include "token.h"

class InitVarException : public std::exception {
 public:
  explicit InitVarException(const std::string &var) : var(var){};

  std::string var;

  virtual const char *what() const throw() {
    std::ostringstream oss;
    oss << "Variable " << var << " already exists in top scope\n";
    return oss.str().c_str();
  }
};

class NoVarException : public std::exception {
 public:
  explicit NoVarException(const std::string &var) : var(var){};

  std::string var;

  virtual const char *what() const throw() {
    std::ostringstream oss;
    oss << "Variable " << var << " does not exist\n";
    std::string str = std::string(oss.str());
    std::cout << str;
    return "";
  }
};

class NoSuchOperatorException : public std::exception {
 public:
  explicit NoSuchOperatorException(const std::string &message)
      : message(message){};

  std::string message;

  virtual const char *what() const throw() { return message.c_str(); }
};

class DivideByZeroException : public std::exception {
 public:
  explicit DivideByZeroException(const Token &location) : location(location){};

  Token location;

  virtual const char *what() const throw() {
    std::ostringstream oss;
    oss << "Divide by Zero at line " << location.get_line() << ", col "
        << location.get_column();
    return oss.str().c_str();
  }
};

class InvalidArgsException : public std::exception {
 public:
  explicit InvalidArgsException(const std::string &message)
      : message(message) {}

  std::string message;

  virtual const char *what() const throw() { return message.c_str(); }
};

class InvalidKeyException : public std::exception {
 public:
  explicit InvalidKeyException(const std::string &message) : message(message) {}

  std::string message;

  virtual const char *what() const throw() { return message.c_str(); }
};

#endif