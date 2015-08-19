#pragma once

#include <string>
#include <memory>
#include <boost/variant.hpp>
#include "ast.hh"

class Lambda;
class Buildin;

struct nil_t {};

using Val = boost::variant<nil_t, int, float, std::string, ast_t, Lambda, Buildin>;
using ValPtr = std::shared_ptr<Val>;		

std::string to_string(const ValPtr& ptr);
