#include "environment.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "object.h"

using namespace env;

TEST(Environment, Init) {
  env_ptr e = env_ptr(new Environment());
  std::string key = "key";
  int value = 25;
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));

  ASSERT_NO_THROW(e->init(key, int_obj));
  ASSERT_ANY_THROW(e->init(key, int_obj))
      << "Init should not initialize the same variable in the same environment "
         "level";
}

TEST(Environment, Get) {
  env_ptr e = env_ptr(new Environment());
  std::string key = "key";
  int value = 25;
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));
  e->init(key, int_obj);

  ASSERT_NO_THROW(e->get(key));
  ASSERT_TRUE(e->get(key) == int_obj) << "::get() should return correct object";
  ASSERT_NO_THROW(std::dynamic_pointer_cast<obj::Integer>(e->get(key)))
      << "Object should be correct type";
}

TEST(Environment, Set) {
  env_ptr e = env_ptr(new Environment());
  std::string key = "key";
  int value = 25;
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));
  int new_value = 50;
  obj::obj_ptr new_int_obj = obj::int_ptr(new obj::Integer(new_value));
  e->init(key, int_obj);

  ASSERT_NO_THROW(e->set(key, new_int_obj));
  ASSERT_TRUE(e->get(key) == new_int_obj)
      << "::set() should cause ::Get() to return correct object";
  ASSERT_NO_THROW(std::dynamic_pointer_cast<obj::Integer>(e->get(key)))
      << "Object should be correct type";
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

TEST(Environment, StackedGet) {
  env_ptr outer = env_ptr(new Environment());
  int value = 25;
  std::string key = "key";
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));
  outer->init(key, int_obj);

  env_ptr inner = env_ptr(new Environment(outer));
  ASSERT_EQ(inner->get(key), int_obj);
}

TEST(Environment, StackedSet) {
  env_ptr outer = env_ptr(new Environment());
  int value = 25;
  std::string key = "key";
  obj::obj_ptr int_obj = obj::int_ptr(new obj::Integer(value));
  outer->init(key, int_obj);

  env_ptr inner = env_ptr(new Environment(outer));
  obj::obj_ptr new_int_obj = obj::int_ptr(new obj::Integer(50));
  ASSERT_NO_THROW(inner->set(key, new_int_obj));
  ASSERT_EQ(inner->get(key), new_int_obj);
  ASSERT_EQ(outer->get(key), new_int_obj);
}