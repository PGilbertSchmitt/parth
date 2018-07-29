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

    case ast::RETURN: {
      ast::return_ptr ret_node = std::dynamic_pointer_cast<ast::Return>(node);
      obj::obj_ptr returned_value = eval(ret_node->expression, envir);
      return obj::return_ptr(new obj::ReturnVal(returned_value));
    } break;

    case ast::INTEGER: {
      ast::int_ptr int_node = std::dynamic_pointer_cast<ast::Integer>(node);
      return evalInteger(int_node);
    } break;

    case ast::BOOLEAN: {
      ast::bool_ptr bool_node = std::dynamic_pointer_cast<ast::Bool>(node);
      return evalBool(bool_node);
    } break;

    case ast::STRING: {
      ast::str_ptr str_node = std::dynamic_pointer_cast<ast::String>(node);
      return evalString(str_node);
    }

    case ast::LIST: {
      ast::arr_ptr arr_node = std::dynamic_pointer_cast<ast::List>(node);
      return evalList(arr_node, envir);
    }

    case ast::FUNCTION: {
      ast::func_ptr func_node = std::dynamic_pointer_cast<ast::Function>(node);
      return evalFunctionLiteral(func_node, envir);
    }

    case ast::PREFIX: {
      ast::prefix_ptr prefix_node =
          std::dynamic_pointer_cast<ast::Prefix>(node);
      return evalPrefix(prefix_node, envir);
    } break;

    case ast::INFIX: {
      ast::infix_ptr infix_node = std::dynamic_pointer_cast<ast::Infix>(node);
      return evalInfix(infix_node, envir);
    } break;

    case ast::IF_ELSE: {
      ast::ifelse_ptr ifelse_node =
          std::dynamic_pointer_cast<ast::IfElse>(node);
      obj::obj_ptr if_else_val = evalIfElse(ifelse_node, envir);
      return if_else_val;
    } break;

    default: {
      return obj::err_ptr(
          new obj::Error("Unknown node type: " + node->to_string() +
                         ", not sure how to evaluate."));
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
      // Casting is required. Attempting to access `value` won't work otherwise.
      // I'm not certain why this is, but I do not question Bjarne Stroustrup
      std::string name =
          std::dynamic_pointer_cast<ast::Option>(let->name)->value;
      envir->init(name, opt);
      return opt;
    } else {
      return right;
    }
  } else {
    obj::opt_ptr opt = NONE_OBJ;
    envir->init(let->name->value, opt);
    return opt;
  }
}

obj::int_ptr evalInteger(ast::int_ptr int_node) {
  return obj::int_ptr(new obj::Integer(int_node->value));
}

obj::bool_ptr evalBool(ast::bool_ptr bool_node) {
  if (bool_node->value) {
    return TRUE_OBJ;
  } else {
    return FALSE_OBJ;
  };
}

obj::str_ptr evalString(ast::str_ptr str_node) {
  return obj::str_ptr(new obj::String(str_node->value));
}

obj::arr_ptr evalList(ast::arr_ptr arr_node, env::env_ptr envir) {
  std::vector<obj::obj_ptr> elements;
  std::vector<ast::node_ptr>::iterator cur_node;
  for (cur_node = arr_node->values.begin(); cur_node != arr_node->values.end();
       cur_node++) {
    elements.push_back(eval(*cur_node, envir));
  }
  return obj::arr_ptr(new obj::List(elements));
}

obj::func_ptr evalFunctionLiteral(ast::func_ptr func_node, env::env_ptr envir) {
  return obj::func_ptr(new obj::Function(func_node, envir));
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

  if (op.get_type() == TokenType::DOUBLE_AMP ||
      op.get_type() == TokenType::DOUBLE_PIPE) {
    obj::bool_ptr left_bool = truthiness(left_eval);
    obj::bool_ptr right_bool = truthiness(right_eval);
    return evalBoolInfixOperator(op, left_bool, right_bool);
  }

  if (left_eval->_type() == obj::INTEGER &&
      right_eval->_type() == obj::INTEGER) {
    obj::int_ptr left = std::dynamic_pointer_cast<obj::Integer>(left_eval);
    obj::int_ptr right = std::dynamic_pointer_cast<obj::Integer>(right_eval);
    return evalIntegerInfixOperator(op, left, right);
  }

  if (left_eval->_type() == obj::BOOLEAN &&
      right_eval->_type() == obj::BOOLEAN) {
    obj::bool_ptr left = std::dynamic_pointer_cast<obj::Bool>(left_eval);
    obj::bool_ptr right = std::dynamic_pointer_cast<obj::Bool>(right_eval);
    return evalBoolInfixOperator(op, left, right);
  }

  if (left_eval->_type() == obj::STRING && right_eval->_type() == obj::STRING) {
    obj::str_ptr left = std::dynamic_pointer_cast<obj::String>(left_eval);
    obj::str_ptr right = std::dynamic_pointer_cast<obj::String>(right_eval);
    return evalStringInfixOperator(op, left, right);
  }

  if (left_eval->_type() == obj::LIST && right_eval->_type() == obj::LIST) {
    obj::arr_ptr left = std::dynamic_pointer_cast<obj::List>(left_eval);
    obj::arr_ptr right = std::dynamic_pointer_cast<obj::List>(right_eval);
    return evalListInfixOperator(op, left, right);
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
    // We will need to do some checks to make sure certain types are cloned so
    // that they aren't passed around by reference (int, float, bool, string)
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
      return nativeBoolToObject(left->value == right->value);
    }
    case TokenType::NEQ: {
      return nativeBoolToObject(left->value != right->value);
    }
    case TokenType::LT: {
      return nativeBoolToObject(left->value < right->value);
    }
    case TokenType::GT: {
      return nativeBoolToObject(left->value > right->value);
    }
    case TokenType::LTEQ: {
      return nativeBoolToObject(left->value <= right->value);
    }
    case TokenType::GTEQ: {
      return nativeBoolToObject(left->value >= right->value);
    }
    default: {
      throw NoSuchOperatorException("No such operation INT " +
                                    op.get_literal() + " INT\n");
    }
  }
}

obj::obj_ptr evalBoolInfixOperator(Token op, obj::bool_ptr left,
                                   obj::bool_ptr right) {
  switch (op.get_type()) {
    case TokenType::EQ: {
      return nativeBoolToObject(left->value == right->value);
    }
    case TokenType::NEQ: {
      return nativeBoolToObject(left->value != right->value);
    }
    case TokenType::DOUBLE_AMP: {
      return nativeBoolToObject(left->value && right->value);
    }
    case TokenType::DOUBLE_PIPE: {
      return nativeBoolToObject(left->value || right->value);
    }
    default: {
      throw NoSuchOperatorException("No such operator BOOLEAN " +
                                    op.get_literal() + " BOOLEAN");
    }
  }
}

obj::obj_ptr evalStringInfixOperator(Token op, obj::str_ptr left,
                                     obj::str_ptr right) {
  switch (op.get_type()) {
    case TokenType::EQ: {
      return nativeBoolToObject(left->value == right->value);
    } break;
    case TokenType::NEQ: {
      return nativeBoolToObject(left->value != right->value);
    } break;
    case TokenType::PLUS: {
      std::string new_string = left->value + right->value;
      std::cout << new_string << "\n";
      return obj::str_ptr(new obj::String(new_string));
    } break;
    default: {
      throw NoSuchOperatorException("No such operator STRING " +
                                    op.get_literal() + " STRING");
    }
  }
}

obj::obj_ptr evalListInfixOperator(Token op, obj::arr_ptr left,
                                   obj::arr_ptr right) {
  switch (op.get_type()) {
    case TokenType::EQ: {
      return nativeBoolToObject(left == right);
    } break;
    case TokenType::NEQ: {
      return nativeBoolToObject(left != right);
    } break;
    case TokenType::PLUS: {
      // Append right list to left list
      obj::internal_list obj_list;
      obj_list.insert(obj_list.end(), left->values.begin(), left->values.end());
      obj_list.insert(obj_list.end(), right->values.begin(),
                      right->values.end());
      return obj::arr_ptr(new obj::List(obj_list));
    } break;
    default: {
      throw NoSuchOperatorException("No such operator LIST " +
                                    op.get_literal() + " LIST");
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

/* Conditional structures are expressions just like everything else, so they
 * have to return something. However, if none of the conditions are true and
 * there's no `else` or default case, they can't return nothing, or NULL, since
 * that doesn't exist in this language. So, in order to enforce uniformity on
 * returns for safe coding, a decision was made to require all conditional
 * expressions to always return optionals. Currently, everything is wrapped in
 * an optional (or a NONE is returned if there is no default), including other
 * optionals. This may change in the future.
 */
obj::opt_ptr evalIfElse(ast::ifelse_ptr ifelse_node, env::env_ptr envir) {
  std::vector<ast::condition_set>::iterator set;
  for (set = ifelse_node->list.begin(); set != ifelse_node->list.end(); set++) {
    bool execute_block = false;
    if (set->condition == NULL) {
      execute_block = true;
    } else {
      obj::obj_ptr condition = eval(set->condition, envir);
      execute_block = truthiness(condition, false) == TRUE_OBJ;
    }

    if (execute_block) {
      obj::obj_ptr consequence = eval(set->consequence, envir);
      return obj::opt_ptr(new obj::Option(consequence));
    }
  }
  return NONE_OBJ;
}

/***********/
/* HELPERS */
/***********/

// Here is where truthiness is determined. Any value is truthy if it's not
// falsy. Any value is falsy if it's one of:
// - false (bool)
// - 0     (int)
// - ""    (string)
// - []    (list)
// - {}    (hash)
// - none  (option)
obj::bool_ptr truthiness(obj::obj_ptr input, bool negate) {
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
    } break;
    case obj::STRING: {
      obj::str_ptr str_obj = std::dynamic_pointer_cast<obj::String>(input);
      new_val = str_obj->value != "";
    } break;
    case obj::LIST: {
      obj::arr_ptr arr_obj = std::dynamic_pointer_cast<obj::List>(input);
      new_val = !(arr_obj->values.size() == 0);
    }
    default: {}
  }
  return nativeBoolToObject(new_val ^ negate);
}

obj::bool_ptr nativeBoolToObject(bool val) {
  return val ? TRUE_OBJ : FALSE_OBJ;
}