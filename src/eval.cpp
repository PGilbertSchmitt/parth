#include "eval.h"

obj::obj_ptr eval(ast::node_ptr node, env::env_ptr envir) {
  switch (node->_type()) {
    case ast::BLOCK: {
      ast::block_ptr block_node = std::dynamic_pointer_cast<ast::Block>(node);
      return evalBlock(block_node, envir);
    } break;

    case ast::IDENT: {
      ast::ident_ptr ident_node =
          std::dynamic_pointer_cast<ast::Identifier>(node);
      return evalIdent(ident_node, envir);
    } break;

    case ast::LET: {
      ast::let_ptr let_node = std::dynamic_pointer_cast<ast::Let>(node);
      return evalLet(let_node, envir);
    } break;

      // case ast::ASSIGN:
      // case ast::RETURN:
      // case ast::INTEGER:

    case ast::BOOLEAN: {
      ast::bool_ptr bool_node = std::dynamic_pointer_cast<ast::Boolean>(node);
      return evalBoolean(bool_node);
    } break;

      // case ast::OPTION:
      // case ast::PREFIX:
      // case ast::INFIX:
      // case ast::IF_ELSE:

    default: {
      return obj::err_ptr(
          new obj::Error("Unknown node type: " + node->to_string()));
    }
  }
}

obj::obj_ptr evalBlock(ast::block_ptr block_node, env::env_ptr envir) {
  obj::obj_ptr result;

  std::vector<ast::node_ptr>::iterator node = block_node->nodes.begin();
  while (node != block_node->nodes.end()) {
    // Dereferencing a smart pointer seems like an oxymoron
    result = eval(*node, envir);

    if (result != nullptr) {
      obj::obj_type result_type = result->_type();
      if (result_type == obj::ERROR || result_type == obj::RETURN_VAL) {
        return result;
      }
    }

    node++;
  }

  return result;
}

obj::obj_ptr evalIdent(ast::ident_ptr ident, env::env_ptr envir) {
  obj::obj_ptr value = envir->get(ident->value);
  if (value != nullptr) {
    return value;
  } else {
    std::cout << "No such identifier " << ident->value
              << ". Must be a builtin?\n";
  }
}

obj::obj_ptr evalLet(ast::let_ptr let, env::env_ptr envir) {
  obj::obj_ptr right = eval(let->expression, envir);
  if (right->_type() == obj::ERROR) {
    return right;
  }
  envir->init(let->name->value, right);
}

obj::bool_ptr evalBoolean(ast::bool_ptr bool_node) {
  if (bool_node->value) {
    return TRUE_OBJ;
  } else {
    return FALSE_OBJ;
  };
}