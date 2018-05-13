#pragma once
#include <string>
#include "token.h"

class Lexer {
 public:
  Lexer(std::string);

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
  bool read_string(std::string &);
  bool dual_char_token(std::string &);
  void skip_whitespace();
  char peak_char();

  static bool isLetter(char);
  static bool isDigit(char);
};