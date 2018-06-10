#ifndef EVAL_H
#define EVAL_H

#include <memory>
#include <vector>
#include "ast.h"
#include "environment.h"
#include "object.h"
#include "parth_error.h"

const obj::bool_ptr TRUE_OBJ = obj::bool_ptr(new obj::Boolean(true));
const obj::bool_ptr FALSE_OBJ = obj::bool_ptr(new obj::Boolean(false));

obj::obj_ptr eval(ast::node_ptr, env::env_ptr);

obj::obj_ptr evalBlock(ast::block_ptr, env::env_ptr);
obj::obj_ptr evalIdent(ast::ident_ptr, env::env_ptr);
obj::obj_ptr evalLet(ast::let_ptr, env::env_ptr);
obj::int_ptr evalInteger(ast::int_ptr);
obj::bool_ptr evalBoolean(ast::bool_ptr);
obj::obj_ptr evalInfix(ast::infix_ptr, env::env_ptr);
obj::obj_ptr evalAssign(ast::ident_ptr, ast::node_ptr, env::env_ptr);
obj::obj_ptr evalIntegerInfixOperator(Token, ast::int_ptr, ast::int_ptr);

#endif