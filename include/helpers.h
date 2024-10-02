#pragma once
#ifndef HELPERS_H
#define HELPERS_H

#include "babyjson.h"
#include <ostream>

auto print_list(std::ostream &os, const JSONList &list) -> void;
auto print_object(std::ostream &os, const JSONDict &obj) -> void;

auto parse_command_line(int argc, char *argv[]) -> void;

#endif //HELPERS_H
