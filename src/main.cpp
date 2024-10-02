#include "babyjson.h"
#include "helpers.h"

#include <iostream>

auto main(const int argc, char *argv[]) -> int {
  try {
    parse_command_line(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
