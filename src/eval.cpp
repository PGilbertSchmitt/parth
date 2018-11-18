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

    case ast::MAP: {
      ast::map_ptr map_node = std::dynamic_pointer_cast<ast::Map>(node);
      return evalMap(map_node, envir);
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

    case ast::GROUP: {
      ast::grp_ptr group_node = std::dynamic_pointer_cast<ast::Group>(node);
      return eval(group_node->expr, envir);
    }

    case ast::CALL: {
      ast::call_ptr call_node = std::dynamic_pointer_cast<ast::Call>(node);
      obj::obj_ptr callable = eval(call_node->function, envir);

      if (callable->_type() != obj::FUNCTION &&
          callable->_type() != obj::BUILTIN) {
        throw NoSuchOperatorException("No call operation on type " +
                                      obj::type_to_string(callable->_type()));
      }

      obj::obj_list args = evalExpressionList(call_node->args, envir);
      return applyFunction(callable, args);
    } break;

    case ast::INDEX: {
      ast::index_ptr index_node = std::dynamic_pointer_cast<ast::Index>(node);
      return evalIndex(index_node->left, index_node->index, envir);
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

  ast::node_list::iterator node = block_node->nodes.begin();
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
  // First gotta check if this ident belongs to a builtin

  if (Builtins::is_builtin(ident->value)) {
    BI bi = Builtins::get_builtin(ident->value);
    return obj::builtin_ptr(new obj::Builtin(bi));
  }

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
  obj::obj_list elements = evalExpressionList(arr_node->values, envir);
  return obj::arr_ptr(new obj::List(elements));
}

obj::map_ptr evalMap(ast::map_ptr map_node, env::env_ptr envir) {
  obj::obj_map evaluated_kvs;

  ast::kv_list::iterator iter;
  for (iter = map_node->key_value_pairs.begin();
       iter < map_node->key_value_pairs.end(); iter++) {
    obj::obj_ptr key_obj, val_obj;
    key_obj = eval(iter->first, envir);
    val_obj = eval(iter->second, envir);

    obj::obj_pair kv_pair;
    kv_pair.first = key_obj;
    kv_pair.second = val_obj;

    uint64_t key_hash = key_obj->hash();
    evaluated_kvs[key_hash] = kv_pair;
  }

  return obj::map_ptr(new obj::Map(evaluated_kvs));
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

  if (left_node->_type() == ast::INDEX && op.get_type() == TokenType::ASSIGN) {
    ast::index_ptr left = std::dynamic_pointer_cast<ast::Index>(left_node);
    return evalIndexAssign(left, right_node, envir);
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
    // that they aren't passed around by reference (int, float, bool, string).
    // This might be unnecessary if these "cloned" types are immutable simply by
    // having no method of changing them. However, it could still be safer to
    // clone explicitly, if a bit inefficient.
    envir->set(left->value, value);
  }
  return value;
}

obj::obj_ptr evalIndexAssign(ast::index_ptr left, ast::node_ptr right,
                             env::env_ptr envir) {
  obj::obj_ptr left_obj = eval(left->left, envir);
  obj::obj_ptr index = eval(left->index, envir);
  obj::obj_ptr value = eval(right, envir);

  switch (left_obj->_type()) {
    case obj::LIST: {
      obj::arr_ptr list_obj = std::dynamic_pointer_cast<obj::List>(left_obj);
      return indexList(list_obj, index, value);
    }
    case obj::MAP: {
      obj::map_ptr map_obj = std::dynamic_pointer_cast<obj::Map>(left_obj);
      return indexMap(map_obj, index, value);
    }
    default: {
      throw NoSuchOperatorException(obj::type_to_string(left_obj->_type()) +
                                    " cannot be indexed using []");
    }
  }

  return value;
}

obj::obj_ptr evalIndex(ast::node_ptr left_expr, ast::node_ptr index_expr,
                       env::env_ptr envir) {
  obj::obj_ptr left_obj = eval(left_expr, envir);
  obj::obj_ptr index_obj = eval(index_expr, envir);

  switch (left_obj->_type()) {
    case obj::LIST: {
      obj::arr_ptr list_obj = std::dynamic_pointer_cast<obj::List>(left_obj);
      return indexList(list_obj, index_obj);
    }
    case obj::STRING: {
      obj::str_ptr str_obj = std::dynamic_pointer_cast<obj::String>(left_obj);
      return indexString(str_obj, index_obj);
    }
    case obj::MAP: {
      obj::map_ptr map_obj = std::dynamic_pointer_cast<obj::Map>(left_obj);
      return indexMap(map_obj, index_obj);
    }
    default: {
      throw NoSuchOperatorException(obj::type_to_string(left_obj->_type()) +
                                    " cannot be indexed using []");
    }
  }
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
      obj::obj_list new_obj_list;
      new_obj_list.insert(new_obj_list.end(), left->values.begin(),
                          left->values.end());
      new_obj_list.insert(new_obj_list.end(), right->values.begin(),
                          right->values.end());
      return obj::arr_ptr(new obj::List(new_obj_list));
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
obj::obj_ptr evalIfElse(ast::ifelse_ptr ifelse_node, env::env_ptr envir) {
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
      // if the consequence is a return type, we do not wrap anything in an
      // option. A return should be used to break out of blocks, so we don't
      // actually care about the optionality of the return value. This means you
      // don't have to unwrap a function's return value unless an if-expression
      // without a default is literally the last one in a block. If that's the
      // case, it's absolutely your own fault for what happens :P
      if (consequence->_type() == obj::RETURN_VAL) {
        return consequence;
      }
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

obj::obj_list evalExpressionList(ast::node_list exprs, env::env_ptr envir) {
  obj::obj_list values;
  ast::node_list::iterator cur_node;
  for (cur_node = exprs.begin(); cur_node != exprs.end(); cur_node++) {
    values.push_back(eval(*cur_node, envir));
  }
  return values;
}

obj::obj_ptr applyFunction(obj::obj_ptr callable, obj::obj_list args) {
  if (callable->_type() == obj::BUILTIN) {
    obj::builtin_ptr builtin =
        std::dynamic_pointer_cast<obj::Builtin>(callable);
    return builtin->fn(args);
  }

  // If not a builtin, can only be a regular function
  obj::func_ptr func_obj = std::dynamic_pointer_cast<obj::Function>(callable);
  ast::param_list params = func_obj->func_node->params;
  if (params.size() != args.size()) {
    throw InvalidArgsException("Incorrect number of args given");
  }

  // This new environment encloses the function's environment, providing
  // temporary access to the new argument variables
  env::env_ptr new_env = env::env_ptr(new env::Environment(func_obj->envir));

  // Filling the new environment with happy argument values.
  ast::param_list::iterator param;
  obj::obj_list::iterator arg_value;
  for (param = params.begin(), arg_value = args.begin(); param != params.end();
       param++, arg_value++) {
    // Man, these pointers are starting to get confusing.
    new_env->init((*param)->value, (*arg_value));
  }

  obj::obj_ptr result = eval(func_obj->func_node->body, new_env);
  return unwrapReturn(result);
}

obj::obj_ptr unwrapReturn(obj::obj_ptr val) {
  if (val->_type() == obj::RETURN_VAL) {
    obj::return_ptr return_obj = std::dynamic_pointer_cast<obj::ReturnVal>(val);
    return return_obj->value;
  }
  return val;
}

obj::obj_ptr indexList(obj::arr_ptr list, obj::obj_ptr index,
                       obj::obj_ptr value) {
  switch (index->_type()) {
    case obj::INTEGER: {
      obj::int_ptr int_obj = std::dynamic_pointer_cast<obj::Integer>(index);
      int64_t ind = int_obj->value;
      if (ind < 0 || static_cast<uint64_t>(ind) >= list->values.size()) {
        return NONE_OBJ;
      }

      // Return value if not an assignment
      if (value != nullptr) {
        return list->values.at(ind);
      }

      // Set list value at index and return passed value if assignment
      list->values.assign(ind, value);
      return value;
    } break;
    // case obj::FUNCTION: {
    //   // TODO: When 'map' builtin is written, use that here
    // } break;
    default: {
      throw NoSuchOperatorException("Cannot use " +
                                    obj::type_to_string(index->_type()) +
                                    " to index list.");
    }
  }
}

obj::obj_ptr indexString(obj::str_ptr str, obj::obj_ptr index) {
  switch (index->_type()) {
    case obj::INTEGER: {
      obj::int_ptr int_obj = std::dynamic_pointer_cast<obj::Integer>(index);
      int64_t val = int_obj->value;
      if (val < 0 || static_cast<uint64_t>(val) >= str->value.size()) {
        return obj::str_ptr(new obj::String(""));
      }
      return obj::str_ptr(new obj::String(str->value.substr(val, 1)));
    } break;
    // case obj::FUNCTION: {
    //   // TODO: When 'map' builtin is written, use that here
    // } break;
    default: {
      throw NoSuchOperatorException("Cannot use " +
                                    obj::type_to_string(index->_type()) +
                                    " to index string.");
    }
  }
}

obj::obj_ptr indexMap(obj::map_ptr map, obj::obj_ptr index,
                      obj::obj_ptr value) {
  uint64_t key_hash = index->hash();

  bool has_key = map->pairs.count(key_hash) > 0;

  if (value == nullptr) {
    if (map->pairs.size() == 0 || !has_key) {
      return NONE_OBJ;
    }

    const obj::obj_pair kv = (map->pairs).at(key_hash);
    return kv.second;
  }

  obj::obj_pair new_kv_pair;
  new_kv_pair.first = index;
  new_kv_pair.second = value;

  (map->pairs).at(key_hash) = new_kv_pair;
  return value;
}

void bust(std::string str) { std::cout << str << std::endl; }