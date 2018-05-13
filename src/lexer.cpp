#include "lexer.h"
#include <iostream>

Lexer::Lexer(const std::string &_input) {
  input = _input;
  position = 0;
  read_position = 0;
  ch = 0;
  len = input.length();
  cur_line = 0;
  cur_column = 0;

  read_char();
}

Token Lexer::next_token() {
  // These defaults mean that these values need to be overwritten
  token_type tok = Token::ILLEGAL;
  std::string lit = "ILLEGAL";
  uint col = cur_column;
  uint line = cur_line;

  skip_whitespace();

  switch (ch) {
    // Simple operators
    case '+':
      tok = Token::PLUS;
      lit = "+";
      break;
    case '-':
      tok = Token::MINUS;
      lit = "-";
      break;
    case '*':
      tok = Token::ASTERISK;
      lit = "*";
      break;
    case '/':
      tok = Token::SLASH;
      lit = "/";
      break;
    case '%':
      tok = Token::MODULO;
      lit = "%";
      break;

    // Delimiters
    case ',':
      tok = Token::COMMA;
      lit = ",";
      break;
    case ':':
      tok = Token::COLON;
      lit = ":";
      break;
    case '\n':
      // This is the only character at the moment that allows tracking of
      bump_line();
      tok = Token::NEWLINE;
      lit = "\n";
      break;

    // Dual char tokens
    case '=':
      if (peak_char() == '=') {
        read_char();
        tok = Token::EQ;
        lit = "==";
      } else {
        tok = Token::ASSIGN;
        lit = "=";
      }
      break;
    case '!':
      if (peak_char() == '=') {
        read_char();
        tok = Token::NEQ;
        lit = "!=";
      } else {
        tok = Token::BANG;
        lit = '!';
      }
      break;
    case '<':
      if (peak_char() == '=') {
        read_char();
        tok = Token::LTEQ;
        lit = "<=";
      } else {
        tok = Token::LT;
        lit = "<";
      }
      break;
    case '>':
      if (peak_char() == '=') {
        read_char();
        tok = Token::GTEQ;
        lit = ">";
      } else {
        tok = Token::GT;
        lit = ">";
      }
      break;

    // Braces
    case '(':
      tok = Token::LPAREN;
      lit = "(";
      break;
    case ')':
      tok = Token::RPAREN;
      lit = ")";
      break;
    case '[':
      tok = Token::LBRACKET;
      lit = "]";
      break;
    case ']':
      tok = Token::RBRACKET;
      lit = "]";
      break;
    case '{':
      tok = Token::LBRACE;
      lit = "{";
      break;
    case '}':
      tok = Token::RBRACE;
      lit = "}";
      break;

    // EOF
    case 0:
      tok = Token::EOF_VAL;
      lit = "";
      break;

    // Strings
    case '"':
      std::string str;
      bool ok = read_string(str);
      if (ok) {
        tok = Token::STRING;
        lit = str;
      } else {
        lit = "Unexpected EOF in string";
      }
      break;

    // Default catches the complex multichar tokens such as numbers and
    // identifiers
    default:
      if (isLetter(ch)) {
        lit = read_ident();
        tok = Token::lookup_ident(lit);
      } else if (isDigit(ch)) {
        lit = read_num();
        tok = Token::INT;
      }
      // If it reaches default case and isn't caught by the above checks, then
      // it's an illegal character and the authorities will be notified.
      // Your days are numbered, criminal.
  }

  return Token(tok, lit, col, line);
}

char Lexer::peak_char() {
  if (read_position >= len) {
    return 0;
  }
  return input[read_position];
};

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

// Returns whether we get a good string or not. If so, then str is set to the
// lexed string value
bool Lexer::read_string(std::string &str) {
  uint pos = position + 1;
  uint size = 0;

  while (true) {
    read_char();
    if (read_position > len) {
      str = "";
      return false;
    }
    if (ch == '"') {
      break;
    }
    // Skip over escaped characters no matter what they are. Either it's a
    // quotation mark, in which we don't want to end the string, or it's not,
    // and we still don't care. It's not the lexer's job.
    if (ch == '\\') {
      read_char();
    }

    size++;
  }

  str = input.substr(pos, size);
  return true;
}

bool Lexer::isDigit(char ch) { return '0' <= ch && ch <= '9'; }

bool Lexer::isLetter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}
void Lexer::bump_line() {
  cur_line++;
  cur_column = 0;
}