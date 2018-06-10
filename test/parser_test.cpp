#include "parser.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "token_type.h"

ast::node_ptr get_first_expression(std::string input) {
  Lexer lexer = Lexer(input);
  Parser parser = Parser(&lexer);
  ast::block_ptr block = parser.parse_program();
  return block->nodes.front();
}

// Prefix Parsing

TEST(Parser, IntTest) {
  ast::node_ptr first = get_first_expression("5");
  ASSERT_EQ(first->_type(), ast::INTEGER);

  ast::int_ptr int_node = std::dynamic_pointer_cast<ast::Integer>(first);
  ASSERT_EQ(int_node->to_string(), "5")
      << "Expected int expression to be '5', got " << int_node->to_string();
}

TEST(Parser, IdentTest) {
  ast::node_ptr first = get_first_expression("var");
  ASSERT_EQ(first->_type(), ast::IDENT);

  ast::ident_ptr ident_node = std::dynamic_pointer_cast<ast::Identifier>(first);
  ASSERT_EQ(ident_node->to_string(), "var")
      << "Expected ident expression to be 'var', got "
      << ident_node->to_string();
}

TEST(Parser, LetTest) {
  ast::node_ptr first = get_first_expression("let x = 5");
  ASSERT_EQ(first->_type(), ast::LET);

  ast::let_ptr let_node = std::dynamic_pointer_cast<ast::Let>(first);
  ASSERT_EQ(let_node->to_string(), "let x = 5")
      << "Expected let expression to be 'let x = 5', got "
      << let_node->to_string();
}

TEST(Parser, ReturnTest) {
  ast::node_ptr first = get_first_expression("return 5");
  ASSERT_EQ(first->_type(), ast::RETURN);

  ast::return_ptr return_node = std::dynamic_pointer_cast<ast::Return>(first);
  ASSERT_EQ(return_node->to_string(), "return 5")
      << "Expected return expression to be 'return 5', got "
      << return_node->to_string();
}

TEST(Parser, MinusTest) {
  ast::node_ptr first = get_first_expression("-13");
  ASSERT_EQ(first->_type(), ast::PREFIX);

  ast::prefix_ptr prefix_node = std::dynamic_pointer_cast<ast::Prefix>(first);
  ASSERT_EQ(prefix_node->to_string(), "(-13)");
  ASSERT_EQ(prefix_node->op.get_literal(), "-");
}

TEST(Parser, BangTest) {
  ast::node_ptr first = get_first_expression("!fact");
  ASSERT_EQ(first->_type(), ast::PREFIX);

  ast::prefix_ptr prefix_node = std::dynamic_pointer_cast<ast::Prefix>(first);
  ASSERT_EQ(prefix_node->to_string(), "(!fact)");
  ASSERT_EQ(prefix_node->op.get_literal(), "!");
}

TEST(Parser, BoolTest) {
  ast::node_ptr first = get_first_expression("true");
  ASSERT_EQ(first->_type(), ast::BOOLEAN);

  ast::bool_ptr true_node = std::dynamic_pointer_cast<ast::Boolean>(first);
  ASSERT_EQ(true_node->to_string(), "true");
  ASSERT_EQ(true_node->value, true);

  ast::node_ptr second = get_first_expression("false");
  ASSERT_EQ(second->_type(), ast::BOOLEAN);

  ast::bool_ptr false_node = std::dynamic_pointer_cast<ast::Boolean>(second);
  ASSERT_EQ(false_node->to_string(), "false");
  ASSERT_EQ(false_node->value, false);
}

TEST(Parser, GroupExpressionTest) {
  ast::node_ptr first = get_first_expression("(4)");
  ASSERT_EQ(first->_type(), ast::INTEGER);

  ast::node_ptr second = get_first_expression("(!myVar)");
  ASSERT_EQ(second->_type(), ast::PREFIX);
}

TEST(Parser, IfElseTest) {
  std::string input = R"INPUT(
if (x) {
  return 0
} else if (y) {
  return 1
} else {
  return 2
}
)INPUT";
  ast::node_ptr first = get_first_expression(input);
  ASSERT_EQ(first->_type(), ast::IF_ELSE);

  ast::ifelse_ptr ifelse_node = std::dynamic_pointer_cast<ast::IfElse>(first);
  ASSERT_EQ(ifelse_node->to_string(),
            "if x { return 0 } else if y { return 1 } else  { return 2 }");
}

// Infix Parsing

struct infix_test_set {
  std::string input;
  std::string to_string;
  std::string op;
};

infix_test_set infix_tests[] = {
    {"a = b", "(a = b)", "="},    {"a + b", "(a + b)", "+"},
    {"a - b", "(a - b)", "-"},    {"a * b", "(a * b)", "*"},
    {"a / b", "(a / b)", "/"},    {"a % b", "(a % b)", "%"},
    {"a < b", "(a < b)", "<"},    {"a > b", "(a > b)", ">"},
    {"a == b", "(a == b)", "=="}, {"a != b", "(a != b)", "!="},
    {"a <= b", "(a <= b)", "<="}, {"a >= b", "(a >= b)", ">="},
};

TEST(Parser, InfixTests) {
  int iterations = sizeof(infix_tests) / sizeof(infix_tests[0]);
  for (int i = 0; i < iterations; i++) {
    infix_test_set cur_test = infix_tests[i];
    std::cout << "Testing infix parse for: " << cur_test.input << std::endl;
    ast::node_ptr node = get_first_expression(cur_test.input);
    ASSERT_EQ(node->_type(), ast::INFIX);

    ast::infix_ptr infix_node = std::dynamic_pointer_cast<ast::Infix>(node);
    ASSERT_EQ(infix_node->to_string(), cur_test.to_string);
    ASSERT_EQ(infix_node->op.get_literal(), cur_test.op);
  }
}