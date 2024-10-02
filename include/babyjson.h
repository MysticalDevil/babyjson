#pragma once

#ifndef BABYJSON_H_
#define BABYJSON_H_

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

struct JSONObject;

using JSONDict = std::unordered_map<std::string, JSONObject>;
using JSONList = std::vector<JSONObject>;

struct JSONObject {
  std::variant<std::nullptr_t, bool, int, double, std::string, JSONList,
               JSONDict>
      inner;

  template <class T> auto is() const -> bool {
    return std::holds_alternative<T>(inner);
  }

  template <class T> auto get() const -> const T & {
    return std::get<T>(inner);
  }

  template <class T> auto get() -> T & { return std::get<T>(inner); }

  friend std::ostream &operator<<(std::ostream &os, const JSONObject &obj);
};

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;
template <Numeric T>
auto try_parse_num(std::string_view str) -> std::optional<T>;

auto unescape_char(char c) -> char;

auto parse_number(std::string_view json) -> std::pair<JSONObject, size_t>;
auto parse_string(std::string_view json) -> std::pair<JSONObject, size_t>;
auto parse_list(std::string_view json) -> std::pair<JSONObject, size_t>;
auto parse_object(std::string_view json) -> std::pair<JSONObject, size_t>;
auto parse(std::string_view json) -> std::pair<JSONObject, size_t>;

#endif // BABYJSON_H_
