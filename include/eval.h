#ifndef EVAL_H
#define EVAL_H

#include <memory>
#include <vector>
#include "ast.h"
#include "environment.h"
#include "object.h"

const obj::bool_ptr TRUE_OBJ = obj::bool_ptr(new obj::Boolean(true));
const obj::bool_ptr FALSE_OBJ = obj::bool_ptr(new obj::Boolean(false));

obj::obj_ptr eval(ast::node_ptr, env::env_ptr);

obj::obj_ptr evalBlock(ast::block_ptr);
obj::bool_ptr evalBoolean(ast::bool_ptr);

#endif