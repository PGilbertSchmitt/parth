#include "environment.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "object.h"

using namespace env;

TEST(Environment, Init) {
  env_ptr e = env_ptr(new Environment());
  int value = 25;
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));

  ASSERT_NO_THROW(e->init("key", int_obj));
  ASSERT_ANY_THROW(e->init("key", int_obj))
      << "Init should not initialize the same variable in the same environment "
         "level";
}

TEST(Environment, StackedInit) {
  env_ptr outer = env_ptr(new Environment());
  int value = 25;
  std::string key = "key";
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));
  outer->init(key, int_obj);

  env_ptr inner = env_ptr(new Environment(outer));
  int new_val = 50;
  obj::obj_ptr new_int = obj::int_ptr(new obj::Integer(new_val));
  ASSERT_NO_THROW(inner->init(key, new_int));
}

// TEST(Environment, Set)