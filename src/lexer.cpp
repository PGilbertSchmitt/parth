#include "lexer.h"
#include <iostream>

Lexer::Lexer(const std::string &_input) {
  input = _input;
  position = 0;
  read_position = 0;
  ch = 0;
  len = input.length();
  cur_line = 1;
  cur_column = 0;

  read_char();
}

Token Lexer::next_token() {
  // These defaults mean that these values need to be overwritten
  TokenType tok = TokenType::ILLEGAL;
  std::string lit = "ILLEGAL";
  uint col = cur_column;
  uint line = cur_line;

  skip_whitespace();

  switch (ch) {
    // Simple operators
    case '+':
      tok = TokenType::PLUS;
      lit = "+";
      break;
    case '-':
      tok = TokenType::MINUS;
      lit = "-";
      break;
    case '*':
      tok = TokenType::ASTERISK;
      lit = "*";
      break;
    case '/':
      tok = TokenType::SLASH;
      lit = "/";
      break;
    case '%':
      tok = TokenType::MODULO;
      lit = "%";
      break;
    case '^':
      tok = TokenType::CARET;
      lit = "^";
      break;

    // Delimiters
    case ',':
      tok = TokenType::COMMA;
      lit = ",";
      break;
    case ':':
      tok = TokenType::COLON;
      lit = ":";
      break;
    case '\n':
      // This is the only character at the moment that allows tracking of lines
      // and columns
      bump_line();
      tok = TokenType::NEWLINE;
      lit = "\n";
      break;

    // Dual char tokens
    case '&':
      if (peak_char() == '&') {
        read_char();
        tok = TokenType::DOUBLE_AMP;
        lit = "&&";
      } else {
        tok = TokenType::AMP;
        lit = "&";
      }
      break;
    case '|':
      if (peak_char() == '|') {
        read_char();
        tok = TokenType::DOUBLE_PIPE;
        lit = "||";
      } else {
        tok = TokenType::PIPE;
        lit = "|";
      }
      break;
    case '=':
      if (peak_char() == '=') {
        read_char();
        tok = TokenType::EQ;
        lit = "==";
      } else if (peak_char() == '>') {
        read_char();
        tok = TokenType::FAT_ARROW;
        lit = "=>";
      } else {
        tok = TokenType::ASSIGN;
        lit = "=";
      }
      break;
    case '!':
      if (peak_char() == '=') {
        read_char();
        tok = TokenType::NEQ;
        lit = "!=";
      } else {
        tok = TokenType::BANG;
        lit = '!';
      }
      break;
    case '<':
      if (peak_char() == '=') {
        read_char();
        tok = TokenType::LTEQ;
        lit = "<=";
      } else {
        tok = TokenType::LT;
        lit = "<";
      }
      break;
    case '>':
      if (peak_char() == '=') {
        read_char();
        tok = TokenType::GTEQ;
        lit = ">=";
      } else {
        tok = TokenType::GT;
        lit = ">";
      }
      break;

    // Braces
    case '(':
      tok = TokenType::LPAREN;
      lit = "(";
      break;
    case ')':
      tok = TokenType::RPAREN;
      lit = ")";
      break;
    case '[':
      tok = TokenType::LBRACKET;
      lit = "]";
      break;
    case ']':
      tok = TokenType::RBRACKET;
      lit = "]";
      break;
    case '{':
      tok = TokenType::LBRACE;
      lit = "{";
      break;
    case '}':
      tok = TokenType::RBRACE;
      lit = "}";
      break;

    // EOF
    case 0:
      tok = TokenType::EOF_VAL;
      lit = "";
      break;

    // Strings
    case '"': {
      std::string str = "";
      bool ok = read_string(str);
      if (ok) {
        tok = TokenType::STRING;
        lit = str;
      } else {
        lit = "Unexpected EOF in string";
      }
      break;
    }

    // Default catches the complex multichar tokens such as numbers and
    // identifiers
    default:
      if (isLetter(ch)) {
        lit = read_ident();
        tok = Token::lookup_ident(lit);
        if (tok == TokenType::OPTION) {
          // Strip the `?`
          lit = lit.substr(0, lit.length() - 1);
        }
      } else if (isDigit(ch)) {
        lit = read_num();
        tok = TokenType::INT;
      }
      break;
      // If it reaches default case and isn't caught by the above checks, then
      // it's an illegal character and the authorities will be notified.
      // Your days are numbered, criminal.
  }

  read_char();
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
  uint size = 1;

  while (isDigit(peak_char())) {
    read_char();
    size++;
  }
  return input.substr(pos, size);
}

std::string Lexer::read_ident() {
  uint pos = position;
  uint size = 1;

  while (isLetter(peak_char()) || isDigit(peak_char())) {
    read_char();
    size++;
  }
  // Grab optionals
  if (peak_char() == '?') {
    read_char();
    size++;
  }
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
      size++;
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