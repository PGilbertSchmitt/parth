
#include <iostream>
#include "ast.h"
#include "environment.h"
#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"

int main() {
  std::string input = R"INPUT(
print("int:", 1)

print("bool:", false)

print("string")

print("list:", [0, 1, 2])

let m = {
  foo: "bar",
  baz: "flu"
}
print("map:", m)

let f = (a, b) => {
  c = a * a
  c + b
}
print("func:", f)

let havent?
print("none:", havent)

let have? = 9
print("opt:", have)

let r1 = (3 .. 9)
print("range 1:", r1)

let r2 = 4...10
print("range 2:", r2)

let r3 = 12..3
print("range 3:", r3)

let r4 = 11...5
print("range 4:", r4)
)INPUT";

  Lexer l = Lexer(input);
  Parser p = Parser(&l);
  ast::block_ptr program = p.parse_program();
  // std::cout << program->to_string() << std::endl;
  env::env_ptr envir = env::env_ptr(new env::Environment());

  obj::obj_ptr end = eval(program, envir);

  std::cout << "Result: " << end->inspect() << std::endl;
}