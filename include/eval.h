#ifndef EVAL_H
#define EVAL_H

#include <memory>
#include <vector>
#include "ast.h"
#include "builtin.h"
#include "environment.h"
#include "object.h"
#include "parth_error.h"
#include "util.h"

obj::obj_ptr eval(ast::node_ptr, env::env_ptr);

obj::obj_ptr evalBlock(ast::block_ptr, env::env_ptr);
obj::obj_ptr evalIdent(ast::ident_ptr, env::env_ptr);
obj::obj_ptr evalLet(ast::let_ptr, env::env_ptr);
obj::obj_ptr evalIdentLet(ast::let_ptr, env::env_ptr);
obj::obj_ptr evalOptLet(ast::let_ptr, env::env_ptr);
obj::int_ptr evalInteger(ast::int_ptr);
obj::bool_ptr evalBool(ast::bool_ptr);
obj::opt_ptr evalOption(ast::opt_ptr, env::env_ptr);
obj::str_ptr evalString(ast::str_ptr);
obj::arr_ptr evalList(ast::arr_ptr, env::env_ptr);
obj::map_ptr evalMap(ast::map_ptr, env::env_ptr);
obj::func_ptr evalFunctionLiteral(ast::func_ptr, env::env_ptr);
obj::obj_ptr evalInfix(ast::infix_ptr, env::env_ptr);
obj::obj_ptr evalPrefix(ast::prefix_ptr, env::env_ptr);
obj::obj_ptr evalAssign(ast::ident_ptr, ast::node_ptr, env::env_ptr);
obj::obj_ptr evalIndex(ast::node_ptr, ast::node_ptr, env::env_ptr);
obj::obj_ptr evalIndexAssign(ast::index_ptr, ast::node_ptr, env::env_ptr);

obj::obj_ptr evalIntegerInfixOperator(Token, obj::int_ptr, obj::int_ptr);
obj::obj_ptr evalBoolInfixOperator(Token, obj::bool_ptr, obj::bool_ptr);
obj::obj_ptr evalStringInfixOperator(Token, obj::str_ptr, obj::str_ptr);
obj::obj_ptr evalListInfixOperator(Token, obj::arr_ptr, obj::arr_ptr);
obj::obj_ptr evalBangOperator(obj::obj_ptr);
obj::obj_ptr evalMinusOperator(obj::int_ptr);
obj::obj_ptr evalIfElse(ast::ifelse_ptr, env::env_ptr);
obj::bool_ptr truthiness(obj::obj_ptr, bool = false);
obj::bool_ptr nativeBoolToObject(bool);
obj::obj_list evalExpressionList(ast::node_list, env::env_ptr);
obj::obj_ptr applyFunction(obj::obj_ptr, obj::obj_list);
obj::obj_ptr unwrapReturn(obj::obj_ptr);
obj::obj_ptr indexList(obj::arr_ptr, obj::obj_ptr, obj::obj_ptr = nullptr);
obj::obj_ptr indexString(obj::str_ptr, obj::obj_ptr);
obj::obj_ptr indexMap(obj::map_ptr, obj::obj_ptr, obj::obj_ptr = nullptr);

#endif