#include "babyjson.h"

#include <charconv>
#include <optional>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <vector>

auto unescape_char(char c) -> char {
  switch (c) {
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case 'f':
      return '\f';
    case 'b':
      return '\b';
    case 'a':
      return '\a';
    case '0':
      return '\0';
    default:
      return c;
  }
}

template<Numeric T>
auto try_parse_num(std::string_view str) -> std::optional<T> {
  T value;
  if (auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    res.ec == std::errc() && res.ptr == str.data() + str.size()) {
    return value;
  }
  return std::nullopt;
}

template auto try_parse_num<int>(std::string_view) -> std::optional<int>;

template auto try_parse_num<double>(std::string_view) -> std::optional<double>;

auto parse_number(std::string_view json) -> std::pair<JSONObject, size_t> {
  const std::regex num_reg{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
  if (std::cmatch match; std::regex_search(json.data(), json.data() + json.size(), match, num_reg)) {
    const std::string str = match.str();
    if (auto num = try_parse_num<int>(str)) {
      return {JSONObject{*num}, str.size()};
    }
    if (auto num = try_parse_num<double>(str)) {
      return {JSONObject{*num}, str.size()};
    }
  }
  return {JSONObject{nullptr}, 0};
}

auto parse_string(std::string_view json) -> std::pair<JSONObject, size_t> {
  std::string str;
  bool escaped = false; // 用于表示是否处于转义状态
  size_t i;

  for (i = 1; i < json.size(); i++) {
    const char ch = json[i];

    if (!escaped) {
      if (ch == '\\') {
        escaped = true;
      } else if (ch == '"') {
        i += 1;
        break;
      } else {
        str += ch;
      }
    } else {
      str += unescape_char(ch);
      escaped = false;
    }
  }

  return {JSONObject{std::move(str)}, i};
}

auto parse_list(std::string_view json) -> std::pair<JSONObject, size_t> {
  std::vector<JSONObject> res;
  size_t i;
  for (i = 1; i < json.size();) {
    if (json[i] == ']') {
      i += 1;
      break;
    }
    auto [obj, eaten] = parse(json.substr(i));
    if (eaten == 0) {
      i = 0;
      break;
    }
    res.push_back(std::move(obj));
    i += eaten;
    if (json[i] == ',') {
      i += 1;
    }
  }
  return {JSONObject{std::move(res)}, i};
}

auto parse_object(std::string_view json) -> std::pair<JSONObject, size_t> {
  std::unordered_map<std::string, JSONObject> res;
  size_t i;
  for (i = 1; i < json.size();) {
    if (json[i] == '}') {
      i += 1;
      break;
    }
    auto [keyobj, keyeaten] = parse(json.substr(i));
    if (keyeaten == 0) {
      i = 0;
      break;
    }
    i += keyeaten;
    if (!std::holds_alternative<std::string>(keyobj.inner)) {
      i = 0;
      break;
    }
    if (json[i] == ':') {
      i += 1;
    }
    std::string key = std::move(std::get<std::string>(keyobj.inner));
    auto [valobj, valeaten] = parse(json.substr(i));
    if (valeaten == 0) {
      i = 0;
      break;
    }
    i += valeaten;
    res.try_emplace(std::move(key), std::move(valobj));
    if (json[i] == ',') {
      i += 1;
    }
  }
  return {JSONObject{std::move(res)}, i};
}


auto parse(const std::string_view json) -> std::pair<JSONObject, size_t> {
  if (json.empty()) {
    return {JSONObject{nullptr}, 0};
  }

  if (const size_t off = json.find_first_not_of(" \n\r\t\v\f\0");
    off != 0 && off != std::string_view::npos) {
    auto [obj, eaten] = parse(json.substr(off));
    return {std::move(obj), eaten + off};
  }

  if (const auto first_char = json.at(0);
    first_char >= '0' && first_char <= '9' || first_char == '+' || first_char == '-') {
    return parse_number(json);
  } else {
    if (first_char == '"') {
      return parse_string(json);
    }
    if (first_char == '[') {
      return parse_list(json);
    }
    if (first_char == '{') {
      return parse_object(json);
    }
  }
  return {JSONObject{std::nullptr_t{}}, 0};
}
