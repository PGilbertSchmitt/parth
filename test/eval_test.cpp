#include "eval.h"
#include <gtest/gtest.h>
#include <memory>
#include "ast.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"

obj::obj_ptr test_eval(const std::string &input) {
  Lexer lexer = Lexer(input);
  Parser parser = Parser(&lexer);
  ast::block_ptr program = parser.parse_program();
  env::env_ptr envir = env::env_ptr(new env::Environment());
  return eval(program, envir);
}

// No need to test the minus op separately
TEST(Eval, IntEval) {
  struct test_suite {
    std::string input;
    int64_t expected;
  };

  test_suite tests[] = {{"5", 5},
                        {"10", 10},
                        {"-5", -5},
                        {"-10", -10},
                        {"5 + 5 + 5 + 5 - 10", 10},
                        {"2 * 2 * 2 * 2 * 2", 32},
                        {"-50 + 100 + -50", 0},
                        {"5 * 2 + 10", 20},
                        {"5 + 2 * 10", 25},
                        {"20 + 2 * -10", 0},
                        {"50 / 2 * 2 + 10", 60},
                        {"2 * (5 + 10)", 30},
                        {"3 * 3 * 3 + 10", 37},
                        {"3 * (3 * 3) + 10", 37},
                        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}};

  int iterations = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < iterations; i++) {
    test_suite cur_test = tests[i];
    std::cout << "Testing eval of " << cur_test.input << std::endl;
    obj::obj_ptr eval_obj = test_eval(cur_test.input);
    ASSERT_EQ(eval_obj->_type(), obj::INTEGER);
    obj::int_ptr eval_int = std::dynamic_pointer_cast<obj::Integer>(eval_obj);
    ASSERT_EQ(cur_test.expected, eval_int->value);
  }
}

TEST(Eval, BoolEval) {
  struct test_suite {
    std::string input;
    bool expected;
  };

  test_suite tests[] = {{"true", true},
                        {"false", false},
                        {"1 < 2", true},
                        {"1 > 2", false},
                        {"1 < 1", false},
                        {"1 > 1", false},
                        {"1 == 1", true},
                        {"1 != 1", false},
                        {"1 == 2", false},
                        {"1 != 2", true},
                        {"true == true", true},
                        {"false == false", true},
                        {"true == false", false},
                        {"true != false", true},
                        {"false != true", true},
                        {"(1 < 2) == true", true},
                        {"(1 < 2) == false", false},
                        {"(1 > 2) == true", false},
                        {"(1 > 2) == false", true}};

  int iterations = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < iterations; i++) {
    test_suite cur_test = tests[i];
    std::cout << "Testing eval of " << cur_test.input << std::endl;
    obj::obj_ptr eval_obj = test_eval(cur_test.input);
    ASSERT_EQ(eval_obj->_type(), obj::BOOLEAN);
    obj::bool_ptr eval_bool = std::dynamic_pointer_cast<obj::Bool>(eval_obj);
    ASSERT_EQ(cur_test.expected, eval_bool->value);
  }
}

TEST(Eval, BangEval) {
  struct test_suite {
    std::string input;
    bool expected;
  };

  test_suite tests[] = {{"!true", false}, {"!false", true},   {"!5", false},
                        {"!!true", true}, {"!!false", false}, {"!!5", true},
                        {"!0", true},     {"!!0", false}};

  int iterations = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < iterations; i++) {
    test_suite cur_test = tests[i];
    std::cout << "Testing eval of " << cur_test.input << std::endl;
    obj::obj_ptr eval_obj = test_eval(cur_test.input);
    ASSERT_EQ(eval_obj->_type(), obj::BOOLEAN);
    obj::bool_ptr eval_bool = std::dynamic_pointer_cast<obj::Bool>(eval_obj);
    ASSERT_EQ(cur_test.expected, eval_bool->value)
        << "Failed on test " << i + 1;
  }
}

TEST(Eval, OptionEval) {
  std::string input = "let opt? = 5";

  std::cout << "Testing eval of " << input << std::endl;
}