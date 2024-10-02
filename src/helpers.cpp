#include "babyjson.h"
#include <ostream>
#include <fstream>
#include <getopt.h>
#include <iostream>

auto print_list(std::ostream &os, const JSONList &list) -> void {
  os << "[";
  for (size_t i = 0; i < list.size(); ++i) {
    os << list[i];
    if (i < list.size() - 1) {
      os << ", ";
    }
  }
  os << "]";
}

auto print_dict(std::ostream &os, const JSONDict &dict) -> void {
  os << "{";
  auto it = dict.begin();
  while (it != dict.end()) {
    os << "\"" << it->first << "\": " << it->second;
    if (++it != dict.end()) {
      os << ", ";
    }
  }
  os << "}";
}

auto operator<<(std::ostream &os, const JSONObject &obj) -> std::ostream & {
  std::visit(
    [&os]<typename T0>(T0 &&value) {
      using T = std::decay_t<T0>;
      if constexpr (std::is_same_v<T, std::nullptr_t>) {
        os << "null";
      } else if constexpr (std::is_same_v<T, bool>) {
        os << (value ? "true" : "false");
      } else if constexpr (std::is_same_v<T, int> ||
                           std::is_same_v<T, double>) {
        os << value;
      } else if constexpr (std::is_same_v<T, std::string>) {
        os << "\"" << value << "\"";
      } else if constexpr (std::is_same_v<T, JSONList>) {
        print_list(os, value);
      } else if constexpr (std::is_same_v<T, JSONDict>) {
        print_dict(os, value);
      }
    },
    obj.inner);
  return os;
}

auto parse_command_line(int argc, char *argv[]) -> void {
  std::string json_content;
  std::string filename;
  bool file_mode = false;

  option long_options[]{
    {"file", required_argument, nullptr, 'f'},
    {nullptr, 0, nullptr, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "f:", long_options, nullptr)) != -1) {
    if (opt == 'f') {
      filename = optarg;
      file_mode = true;
    } else {
      std::cerr << "Usage: " << argv[0] << " [-f/--file <filename>] [json_string]" << std::endl;
    }
  }

  if (file_mode) {
    std::ifstream file(filename);
    if (!file) {
      std::cerr << "Error: Could not open file " << filename << std::endl;
      exit(EXIT_FAILURE);
    }

    json_content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();
  } else {
    if (optind < argc) {
      json_content = argv[optind];
    } else {
      std::cerr << "Please provide a JSON input through command-line or pipe." << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  auto [obj, _] = parse(json_content);
  std::cout << "Parse JSON successfully!" << std::endl;
  std::cout << obj << std::endl;
}
