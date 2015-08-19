#pragma once

#include "lispy.hh"

ValPtr eval(const ast_t& tree, bool qexpr_as_sexpr = false);
