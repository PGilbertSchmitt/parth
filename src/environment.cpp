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

obj::obj_ptr Environment::get(const std::string key) {
  if (this->store.find(key) != this->store.end()) {
    return this->store[key];
  } else {
    throw NoVarException(key);
  }
}