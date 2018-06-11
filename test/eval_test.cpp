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

TEST(Eval, IntEval) {
  struct test_suite {
    std::string input;
    int64_t expected;
  };

  test_suite tests[] = {{"5", 5},
                        {"10", 10},
                        // {"-5", -5},
                        // {"-10", -10},
                        {"5 + 5 + 5 + 5 - 10", 10},
                        {"2 * 2 * 2 * 2 * 2", 32},
                        // {"-50 + 100 + -50", 0},
                        {"5 * 2 + 10", 20},
                        {"5 + 2 * 10", 25},
                        // {"20 + 2 * -10", 0},
                        {"50 / 2 * 2 + 10", 60},
                        {"2 * (5 + 10)", 30},
                        {"3 * 3 * 3 + 10", 37},
                        {"3 * (3 * 3) + 10", 37}};
  // {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}};

  int iterations = sizeof(tests) / sizeof(tests[0]);
  for (int i = 0; i < iterations; i++) {
    test_suite cur_test = tests[i];
    obj::obj_ptr eval_obj = test_eval(cur_test.input);
    obj::int_ptr eval_int = std::dynamic_pointer_cast<obj::Integer>(eval_obj);
    std::cout << "Testing eval of " << cur_test.input << std::endl;
    ASSERT_EQ(cur_test.expected, eval_int->value);
  }
}
