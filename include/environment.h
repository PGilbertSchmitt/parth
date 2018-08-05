#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <memory>
#include <string>
#include <unordered_map>
#include "object.h"
#include "parth_error.h"

namespace env {

class Environment;

typedef std::shared_ptr<Environment> env_ptr;

class Environment {
 public:
  Environment();
  Environment(env_ptr);
  std::unordered_map<std::string, obj::obj_ptr> store;
  env_ptr outer;

  void init(const std::string&, obj::obj_ptr);
  void set(const std::string&, obj::obj_ptr);
  obj::obj_ptr get(const std::string);
  void inspect();
};

}  // namespace env

#endif