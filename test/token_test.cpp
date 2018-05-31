#include "token.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

namespace {

TEST(TokenTest, Keywords) {
  TokenType fn = TokenType::FUNCTION;
  std::string fn_str = "fn";
  ASSERT_EQ(Token::lookup_ident(fn_str), fn);
}

TEST(TokenTest, Ident) {
  std::string ident = "myVar";
  ASSERT_EQ(Token::lookup_ident(ident), TokenType::IDENT);
}

}  // namespace