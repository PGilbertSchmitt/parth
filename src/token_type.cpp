#include "token_type.h"

std::string token_type_string(TokenType type) {
  switch (type) {
    case TokenType::NONE:
      return "NONE";
      break;
    case TokenType::ILLEGAL:
      return "ILLEGAL";
      break;
    case TokenType::EOF_VAL:
      return "EOF_VAL";
      break;
    case TokenType::IDENT:
      return "IDENT";
      break;
    case TokenType::INT:
      return "INT";
      break;
    case TokenType::STRING:
      return "STRING";
      break;
    case TokenType::ASSIGN:
      return "ASSIGN";
      break;
    case TokenType::PLUS:
      return "PLUS";
      break;
    case TokenType::MINUS:
      return "MINUS";
      break;
    case TokenType::SLASH:
      return "SLASH";
      break;
    case TokenType::ASTERISK:
      return "ASTERISK";
      break;
    case TokenType::BANG:
      return "BANK";
      break;
    case TokenType::MODULO:
      return "MODULO";
      break;
    case TokenType::LT:
      return "LT";
      break;
    case TokenType::GT:
      return "GT";
      break;
    case TokenType::EQ:
      return "EQ";
      break;
    case TokenType::NEQ:
      return "NEQ";
      break;
    case TokenType::LTEQ:
      return "LTEQ";
      break;
    case TokenType::GTEQ:
      return "GTEQ";
      break;
    case TokenType::COMMA:
      return "COMMA";
      break;
    case TokenType::COLON:
      return "COLON";
      break;
    case TokenType::NEWLINE:
      return "NEWLINE";
      break;
    case TokenType::FUNCTION:
      return "FUNCTION";
      break;
    case TokenType::LET:
      return "LET";
      break;
    case TokenType::IF:
      return "IF";
      break;
    case TokenType::ELSE:
      return "ELSE";
      break;
    case TokenType::RETURN:
      return "RETURN";
      break;
    case TokenType::TRUE_VAL:
      return "TRUE_VAL";
      break;
    case TokenType::FALSE_VAL:
      return "FALSE_VAL";
      break;
    case TokenType::LPAREN:
      return "LPAREN";
      break;
    case TokenType::RPAREN:
      return "RPAREN";
      break;
    case TokenType::LBRACE:
      return "LBRACE";
      break;
    case TokenType::RBRACE:
      return "RBRACE";
      break;
    case TokenType::LBRACKET:
      return "LBRACKET";
      break;
    case TokenType::RBRACKET:
      return "RBRACKET";
      break;
    default:
      return "NO SUCH TOKEN";
      break;
  }
}