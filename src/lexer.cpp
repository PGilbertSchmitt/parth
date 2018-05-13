#include "lexer.h"
#include <iostream>

Lexer::Lexer(std::string _input) {
  input = _input;
  position = 0;
  read_position = 0;
  ch = 0;
  len = input.length();
  cur_line = 0;
  cur_column = 0;

  read_char();
}

void Lexer::read_char() {
  if (read_position >= len) {
    ch = 0;
  } else {
    ch = input[read_position];
  }

  position = read_position;
  read_position++;
  cur_column++;
}

/* Skips the parsing of whitespace characters altogether, with the exception of
 * hard returns (\n) which are important. Only supporting NL line endings, not
 * CR or CRLF line endings.
 */
void Lexer::skip_whitespace() {
  while (ch == ' ' || ch == '\t' || ch == '\r') {
    read_char();
  }
}

std::string Lexer::read_num() {
  uint pos = position;
  uint size = 0;

  while (isDigit(ch)) {
    read_char();
    size++;
  }

  std::cout << pos << " : " << size << std::endl;
  return input.substr(pos, size);
}

std::string Lexer::read_ident() {
  uint pos = position;
  uint size = 0;

  while (isLetter(ch) || isDigit(ch)) {
    read_char();
    size++;
  }

  std::cout << pos << " : " << size << std::endl;
  return input.substr(pos, size);
}

bool Lexer::isDigit(char ch) { return '0' <= ch && ch <= '9'; }
bool Lexer::isLetter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}