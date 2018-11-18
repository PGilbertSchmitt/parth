
#include <iostream>
#include "ast.h"
#include "environment.h"
#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"

int main() {
  std::string input = R"INPUT(
let a = len("yo")
)INPUT";

  Lexer l = Lexer(input);
  Parser p = Parser(&l);
  ast::block_ptr program = p.parse_program();
  // std::cout << program->to_string() << std::endl;
  env::env_ptr envir = env::env_ptr(new env::Environment());

  obj::obj_ptr end = eval(program, envir);

  std::cout << "Result: " << end->inspect() << std::endl;
}