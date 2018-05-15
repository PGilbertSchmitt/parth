#include "token.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

namespace {

TEST(TokenTest, Keywords) {
  token_type fn = Token::FUNCTION;
  std::string fn_str = "fn";
  ASSERT_TRUE(Token::lookup_ident(fn_str) == fn);
}

TEST(TokenTest, NoKeyword) {
  std::string ident = "myVar";
  ASSERT_TRUE(Token::lookup_ident(ident) == ident);
}

}  // namespace