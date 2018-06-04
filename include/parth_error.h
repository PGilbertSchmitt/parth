#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

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

#endif