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

      // case ast::RETURN:

    case ast::INTEGER: {
      ast::int_ptr int_node = std::dynamic_pointer_cast<ast::Integer>(node);
      return evalInteger(int_node);
    } break;

    case ast::BOOLEAN: {
      ast::bool_ptr bool_node = std::dynamic_pointer_cast<ast::Boolean>(node);
      return evalBoolean(bool_node);
    } break;

    case ast::PREFIX: {
      ast::prefix_ptr prefix_node =
          std::dynamic_pointer_cast<ast::Prefix>(node);
      return evalPrefix(prefix_node, envir);
    } break;

    case ast::INFIX: {
      ast::infix_ptr infix_node = std::dynamic_pointer_cast<ast::Infix>(node);
      return evalInfix(infix_node, envir);
    } break;

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
    return value;
  }
}

obj::obj_ptr evalLet(ast::let_ptr let, env::env_ptr envir) {
  if (let->name->_type() == ast::OPTION) {
    return evalOptLet(let, envir);
  } else {
    return evalIdentLet(let, envir);
  }
}

obj::obj_ptr evalIdentLet(ast::let_ptr let, env::env_ptr envir) {
  obj::obj_ptr right = eval(let->expression, envir);
  if (right->_type() != obj::ERROR) {
    envir->init(let->name->value, right);
  }
  return right;
}

obj::obj_ptr evalOptLet(ast::let_ptr let, env::env_ptr envir) {
  if (let->expression != nullptr) {
    obj::obj_ptr right = eval(let->expression, envir);
    if (right->_type() != obj::ERROR) {
      obj::opt_ptr opt = obj::opt_ptr(new obj::Option(right));
      std::string name =
          std::dynamic_pointer_cast<ast::Option>(let->name)->value;
      std::cout << "Name: " << name << std::endl;
      envir->init(name, opt);
      return opt;
    } else {
      return right;
    }
  } else {
    obj::opt_ptr opt = obj::opt_ptr(new obj::Option());
    envir->init(let->name->value, opt);
    return opt;
  }
}

obj::int_ptr evalInteger(ast::int_ptr int_node) {
  return obj::int_ptr(new obj::Integer(int_node->value));
}

obj::bool_ptr evalBoolean(ast::bool_ptr bool_node) {
  if (bool_node->value) {
    return TRUE_OBJ;
  } else {
    return FALSE_OBJ;
  };
}

obj::obj_ptr evalInfix(ast::infix_ptr infix_node, env::env_ptr envir) {
  // Special cases first
  Token op = infix_node->op;
  ast::node_ptr left_node = infix_node->left;
  ast::node_ptr right_node = infix_node->right;

  if (left_node->_type() == ast::IDENT && op.get_type() == TokenType::ASSIGN) {
    ast::ident_ptr left = std::dynamic_pointer_cast<ast::Identifier>(left_node);
    return evalAssign(left, right_node, envir);
  }

  obj::obj_ptr left_eval = eval(left_node, envir);
  obj::obj_ptr right_eval = eval(right_node, envir);

  if (left_eval->_type() == obj::INTEGER &&
      right_eval->_type() == obj::INTEGER) {
    obj::int_ptr left = std::dynamic_pointer_cast<obj::Integer>(left_eval);
    obj::int_ptr right = std::dynamic_pointer_cast<obj::Integer>(right_eval);
    return evalIntegerInfixOperator(op, left, right);
  }

  if (left_eval->_type() == obj::BOOLEAN &&
      right_eval->_type() == obj::BOOLEAN) {
    obj::bool_ptr left = std::dynamic_pointer_cast<obj::Boolean>(left_eval);
    obj::bool_ptr right = std::dynamic_pointer_cast<obj::Boolean>(right_eval);
    return evalBooleanInfixOperator(op, left, right);
  }

  std::string message =
      "No such operation " + ast::node_type_string(left_node->_type()) + " " +
      op.get_literal() + " " + ast::node_type_string(right_node->_type());
  throw NoSuchOperatorException(message);
}

obj::obj_ptr evalPrefix(ast::prefix_ptr prefix_node, env::env_ptr envir) {
  Token op = prefix_node->op;
  obj::obj_ptr right = eval(prefix_node->right, envir);
  switch (op.get_type()) {
    case TokenType::MINUS: {
      if (right->_type() != obj::INTEGER) {
        throw NoSuchOperatorException("No such operation -(" +
                                      right->inspect() + ")");
      }
      obj::int_ptr int_obj = std::dynamic_pointer_cast<obj::Integer>(right);
      return evalMinusOperator(int_obj);
    }
    case TokenType::BANG: {
      return evalBangOperator(right);
    }
    default:
      std::string message =
          "No such operation " + op.get_literal() + right->inspect();
      throw NoSuchOperatorException(message);
  }
}

obj::obj_ptr evalAssign(ast::ident_ptr left, ast::node_ptr right,
                        env::env_ptr envir) {
  obj::obj_ptr value = eval(right, envir);
  if (value->_type() != obj::ERROR) {
    envir->set(left->value, value);
  }
  return value;
}

obj::obj_ptr evalIntegerInfixOperator(Token op, obj::int_ptr left,
                                      obj::int_ptr right) {
  switch (op.get_type()) {
    case TokenType::PLUS: {
      return obj::int_ptr(new obj::Integer(left->value + right->value));
    }
    case TokenType::MINUS: {
      return obj::int_ptr(new obj::Integer(left->value - right->value));
    }
    case TokenType::ASTERISK: {
      return obj::int_ptr(new obj::Integer(left->value * right->value));
    }
    case TokenType::SLASH: {
      if (right->value == 0) {
        throw DivideByZeroException(op);
      }
      return obj::int_ptr(new obj::Integer(left->value / right->value));
    }
    case TokenType::MODULO: {
      if (right->value == 0) {
        throw DivideByZeroException(op);
      }
      return obj::int_ptr(new obj::Integer(left->value % right->value));
    }
    case TokenType::EQ: {
      return obj::bool_ptr(new obj::Boolean(left->value == right->value));
    }
    case TokenType::NEQ: {
      return obj::bool_ptr(new obj::Boolean(left->value != right->value));
    }
    case TokenType::LT: {
      return obj::bool_ptr(new obj::Boolean(left->value < right->value));
    }
    case TokenType::GT: {
      return obj::bool_ptr(new obj::Boolean(left->value > right->value));
    }
    case TokenType::LTEQ: {
      return obj::bool_ptr(new obj::Boolean(left->value <= right->value));
    }
    case TokenType::GTEQ: {
      return obj::bool_ptr(new obj::Boolean(left->value >= right->value));
    }
    default: {
      throw NoSuchOperatorException("No such operation INT " +
                                    op.get_literal() + " INT\n");
    }
  }
}

obj::obj_ptr evalBooleanInfixOperator(Token op, obj::bool_ptr left,
                                      obj::bool_ptr right) {
  switch (op.get_type()) {
    case TokenType::EQ: {
      return obj::bool_ptr(new obj::Boolean(left->value == right->value));
    }
    case TokenType::NEQ: {
      return obj::bool_ptr(new obj::Boolean(left->value != right->value));
    }
    default: {
      throw NoSuchOperatorException("No such operator BOOLEAN " +
                                    op.get_literal() + "BOOLEAN");
    }
  }
}

obj::obj_ptr evalMinusOperator(obj::int_ptr num) {
  return obj::int_ptr(new obj::Integer(-1 * num->value));
}

obj::obj_ptr evalBangOperator(obj::obj_ptr input) {
  // obj::bool_ptr init_val = truthiness(input);
  return truthiness(input, true);
}

// Here is where truthiness is determined. Any value is truthy if it's not
// falsy. Any value is falsy if it's one of:
// - false (bool)
// - 0     (int)
// - ""    (string)
// - []    (array)
// - {}    (hash)
// - none  (option)
obj::bool_ptr truthiness(obj::obj_ptr input, bool negate = false) {
  bool new_val = false;
  switch (input->_type()) {
    case obj::BOOLEAN: {
      new_val = input == TRUE_OBJ;
    } break;
    case obj::INTEGER: {
      obj::int_ptr int_obj = std::dynamic_pointer_cast<obj::Integer>(input);
      new_val = int_obj->value != 0;
    } break;
    case obj::OPTION: {
      obj::opt_ptr opt_obj = std::dynamic_pointer_cast<obj::Option>(input);
      new_val = opt_obj->value != nullptr;
    }
  }
  return new_val ^ negate ? TRUE_OBJ : FALSE_OBJ;
}