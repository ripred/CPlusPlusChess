/**
 * options.cpp
 *
 * runtime user options
 *
 */

#include <options.h>

#include <iterator>
#include <regex>
#include <string>
#include <vector>

namespace chess {
  using std::regex;
  using std::regex_match;
  using std::smatch;
  using std::ssub_match;
  using std::vector;

  Options::Options(int argc, char **argv) { parse(argc, argv); }

  void Options::clear() { options.clear(); }

  bool Options::parse(int const argc, char const *const *argv) {
    regex option_regex(R"(--([a-zA-Z0-9_]*)[\ \\t]*[=:]?[\ \\t]*([a-zA-Z0-9_\\.]*))");
    smatch match;

    vector<string> args;

    for (int index = 0; index < argc; ++index) {
      args.emplace_back(argv[index]);
    }

    for (auto &arg : args) {
      if (regex_match(arg, match, option_regex)) {
        // The first sub_match is the whole string; the next
        // sub_match is the first parenthesized expression.
        if (match.size() == 3) {
          ssub_match key_sub_match = match[1];
          string key = key_sub_match.str();

          ssub_match value_sub_match = match[2];
          string value = value_sub_match.str();

          if (!key.empty()) {
            if (value.empty()) {
              // treat unary options as 'option = true'
              value = "1";
            }
            options[key] = value;
          }
        }
      }
    }
    return true;
  }

  bool Options::write(string const &filename) {
    std::ofstream ostream(filename, std::ios::binary);
    for (auto &entry : options) {
      ostream << entry.first << std::endl;
      ostream << entry.second << std::endl;
    }

    return true;
  }

  bool Options::read(string const &filename) {
    std::ifstream istream(filename, std::ios::binary);
    options.clear();
    do {
      string key, value;
      istream >> key;
      istream >> value;
      options[key] = value;
    } while (!istream.eof());

    return true;
  }

  bool Options::exists(string &key) const { return options.find(key) != options.end(); }

  bool Options::exists(const char *key) const {
    string k(key);
    return exists(k);
  }

  string Options::get(const char *key, char const *const def) const {
    return (exists(key)) ? options.find(key)->second : def;
  }

  int Options::getInt(const char *const key, int const def) {
    if (!exists(key)) return def;
    string value = options[key];
    if (!isdigit(value[0])) return 0;
    return stoi(value);
  }

  float Options::getFloat(const char *const key, float const def) {
    return exists(key) ? stof(options[key]) : def;
  }

  bool Options::getBool(const char *const key, bool const def) {
    if (!exists(key)) return def;
    string value = options[key];
    if (value.empty()) return def;
    return getInt(key) != 0;
  }

  void Options::set(char const *key, char const *value) { options[string(key)] = string(value); }

  void Options::setInt(char const *key, int const value) {
    options[string(key)] = to_string(value);
  }

  void Options::setFloat(char const *key, float const value) {
    options[string(key)] = to_string(value);
  }

  void Options::setBool(char const *key, bool const value) {
    options[string(key)] = to_string(value);
  }
}  // namespace chess
