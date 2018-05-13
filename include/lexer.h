#pragma once
#include <string>
#include "token.h"

class Lexer {
 public:
  Lexer(const std::string &_input);

  Token next_token();

 private:
  std::string input;
  uint position;
  uint read_position;
  char ch;
  uint len;
  uint cur_line;
  uint cur_column;

  void read_char();
  std::string read_ident();
  std::string read_num();
  bool read_string(std::string &str);
  void skip_whitespace();
  char peak_char();
  void bump_line();

  static bool isLetter(char);
  static bool isDigit(char);
};