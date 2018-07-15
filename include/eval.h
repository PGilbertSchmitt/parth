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
obj::obj_ptr evalIdentLet(ast::let_ptr, env::env_ptr);
obj::obj_ptr evalOptLet(ast::let_ptr, env::env_ptr);
obj::int_ptr evalInteger(ast::int_ptr);
obj::bool_ptr evalBoolean(ast::bool_ptr);
obj::opt_ptr evalOption(ast::opt_ptr, env::env_ptr);
obj::obj_ptr evalInfix(ast::infix_ptr, env::env_ptr);
obj::obj_ptr evalPrefix(ast::prefix_ptr, env::env_ptr);
obj::obj_ptr evalAssign(ast::ident_ptr, ast::node_ptr, env::env_ptr);
obj::obj_ptr evalIntegerInfixOperator(Token, obj::int_ptr, obj::int_ptr);
obj::obj_ptr evalBooleanInfixOperator(Token, obj::bool_ptr, obj::bool_ptr);
obj::obj_ptr evalBangOperator(obj::obj_ptr);
obj::obj_ptr evalMinusOperator(obj::int_ptr);
obj::bool_ptr truthiness(obj::obj_ptr, bool);

#endif