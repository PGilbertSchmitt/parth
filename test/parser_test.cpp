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
  ASSERT_EQ(prefix_node->op, "-");
}

TEST(Parser, BangTest) {
  ast::node_ptr first = get_first_expression("!fact");
  ASSERT_EQ(first->_type(), ast::PREFIX);

  ast::prefix_ptr prefix_node = std::dynamic_pointer_cast<ast::Prefix>(first);
  ASSERT_EQ(prefix_node->to_string(), "(!fact)");
  ASSERT_EQ(prefix_node->op, "!");
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