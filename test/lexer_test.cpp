#include "lexer.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "token.h"

struct test_row {
  TokenType expected_type;
  std::string expected_literal;
};

std::string input = R"INPUT(
let five = 5
let ten = 10
let maybe? = 20

let add = fn(x, y) {
  x + y
}

let result = add(five, ten)
!-/*5
5 < 10 > 5

if (5 < 10) {
	return true
} else {
	return false
}

10 == 10
10 != 9
11 >= 5
3 <= 3
4 % 3

"foobar"
"foo bar"
"foo \"bar\" baz"

if (maybe?) {
  return x
} else {
  let y = x * x
  return -y
}
)INPUT";

test_row all_rows[] = {
    // Tested against
    {TokenType::NEWLINE, "\n"},

    {TokenType::LET, "let"},
    {TokenType::IDENT, "five"},
    {TokenType::ASSIGN, "="},
    {TokenType::INT, "5"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::LET, "let"},
    {TokenType::IDENT, "ten"},
    {TokenType::ASSIGN, "="},
    {TokenType::INT, "10"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::LET, "let"},
    {TokenType::OPTION, "maybe?"},
    {TokenType::ASSIGN, "="},
    {TokenType::INT, "20"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::LET, "let"},
    {TokenType::IDENT, "add"},
    {TokenType::ASSIGN, "="},
    {TokenType::FUNCTION, "fn"},
    {TokenType::LPAREN, "("},
    {TokenType::IDENT, "x"},
    {TokenType::COMMA, ","},
    {TokenType::IDENT, "y"},
    {TokenType::RPAREN, ")"},
    {TokenType::LBRACE, "{"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::IDENT, "x"},
    {TokenType::PLUS, "+"},
    {TokenType::IDENT, "y"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::RBRACE, "}"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::LET, "let"},
    {TokenType::IDENT, "result"},
    {TokenType::ASSIGN, "="},
    {TokenType::IDENT, "add"},
    {TokenType::LPAREN, "("},
    {TokenType::IDENT, "five"},
    {TokenType::COMMA, ","},
    {TokenType::IDENT, "ten"},
    {TokenType::RPAREN, ")"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::BANG, "!"},
    {TokenType::MINUS, "-"},
    {TokenType::SLASH, "/"},
    {TokenType::ASTERISK, "*"},
    {TokenType::INT, "5"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::INT, "5"},
    {TokenType::LT, "<"},
    {TokenType::INT, "10"},
    {TokenType::GT, ">"},
    {TokenType::INT, "5"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::IF, "if"},
    {TokenType::LPAREN, "("},
    {TokenType::INT, "5"},
    {TokenType::LT, "<"},
    {TokenType::INT, "10"},
    {TokenType::RPAREN, ")"},
    {TokenType::LBRACE, "{"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::RETURN, "return"},
    {TokenType::TRUE_VAL, "true"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::RBRACE, "}"},
    {TokenType::ELSE, "else"},
    {TokenType::LBRACE, "{"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::RETURN, "return"},
    {TokenType::FALSE_VAL, "false"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::RBRACE, "}"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::INT, "10"},
    {TokenType::EQ, "=="},
    {TokenType::INT, "10"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::INT, "10"},
    {TokenType::NEQ, "!="},
    {TokenType::INT, "9"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::INT, "11"},
    {TokenType::GTEQ, ">="},
    {TokenType::INT, "5"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::INT, "3"},
    {TokenType::LTEQ, "<="},
    {TokenType::INT, "3"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::INT, "4"},
    {TokenType::MODULO, "%"},
    {TokenType::INT, "3"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::STRING, "foobar"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::STRING, "foo bar"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::STRING, "foo \\\"bar\\\" baz"},
    {TokenType::NEWLINE, "\n"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::IF, "if"},
    {TokenType::LPAREN, "("},
    {TokenType::OPTION, "maybe?"},
    {TokenType::RPAREN, ")"},
    {TokenType::LBRACE, "{"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::RETURN, "return"},
    {TokenType::IDENT, "x"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::RBRACE, "}"},
    {TokenType::ELSE, "else"},
    {TokenType::LBRACE, "{"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::LET, "let"},
    {TokenType::IDENT, "y"},
    {TokenType::ASSIGN, "="},
    {TokenType::IDENT, "x"},
    {TokenType::ASTERISK, "*"},
    {TokenType::IDENT, "x"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::RETURN, "return"},
    {TokenType::MINUS, "-"},
    {TokenType::IDENT, "y"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::RBRACE, "}"},
    {TokenType::NEWLINE, "\n"},

    {TokenType::EOF_VAL, ""},
};

TEST(Lexer, Input) {
  Lexer l = Lexer(input);
  Token cur_token;

  int row_size = sizeof(test_row);
  int test_size = sizeof(all_rows) / row_size;

  for (int i = 0; i < test_size; i++) {
    cur_token = l.next_token();
    test_row cur_row = all_rows[i];

    TokenType cur_type = cur_token.get_type();
    std::string cur_lit = cur_token.get_literal();

    ASSERT_EQ(cur_type, cur_row.expected_type)
        << "Token " << i + 1
        << ": Expected type: " << token_type_string(cur_row.expected_type)
        << " | Got type: " << token_type_string(cur_type);

    ASSERT_EQ(cur_lit, cur_row.expected_literal)
        << "Token " << i + 1
        << ": Expected literal: " << cur_row.expected_literal
        << " | Got literal: " << cur_lit;
  }
}

TEST(Lexer, UnendedString) {
  Lexer l = Lexer("\"No end in sight");
  Token tok = l.next_token();
  ASSERT_EQ(tok.get_type(), TokenType::ILLEGAL)
      << "Expected unended string to create ILLEGAL token, instead got: "
      << token_type_string(tok.get_type());
}