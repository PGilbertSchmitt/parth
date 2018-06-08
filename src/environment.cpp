#include "environment.h"

using namespace env;

Environment::Environment() {}
Environment::Environment(env_ptr outer) : outer(outer) {}

void Environment::init(const std::string &key, obj::obj_ptr value) {
  if (this->store.find(key) == this->store.end()) {
    this->store[key] = value;
  } else {
    throw InitVarException(key);
  }
}

void Environment::set(const std::string &key, obj::obj_ptr value) {
  if (this->store.find(key) != this->store.end()) {
    this->store[key] = value;
  } else if (this->outer != nullptr) {
    this->outer->set(key, value);
  } else {
    std::cout << "Outer exists: " << (this->outer == nullptr) << std::endl;
    throw NoVarException(key);
  }
}

obj::obj_ptr Environment::get(const std::string key) {
  if (this->store.find(key) != this->store.end()) {
    return this->store[key];
  } else if (this->outer != nullptr) {
    return this->outer->get(key);
  } else {
    throw NoVarException(key);
  }
}
