/**
 * options.cpp
 *
 * runtime user options
 *
 */

#include <options.h>
using namespace chess;

Options::Options(int const argc, char** argv) { parse(argc, argv); }

void Options::clear() { options.clear(); }

bool Options::parse(int const argc, char** argv) {
  string lastArg;
  for (auto index = 0; index < argc; ++index) {
    string arg = argv[index];
    int firstChar = arg.empty() ? 0 : arg[0];
    if (firstChar == '=' || firstChar == ':') {
      arg = arg.substr(1);
    }

    int lastChar = arg.empty() ? 0 : *(arg.end() - 1);
    if (lastChar == '=' || lastChar == ':') {
      arg = arg.substr(0, arg.size() - 1);
    }

    if (arg.empty()) continue;

    if (lastArg.empty()) {
      lastArg = arg;
      continue;
    }
    options[lastArg] = arg;
    lastArg.clear();
  }
  if (!lastArg.empty()) {
    options[lastArg] = "";
    lastArg.clear();
  }
  return true;
}

bool Options::write(string const& filename) {
  std::ofstream ostream(filename, std::ios::binary);
  for (auto& entry : options) {
    ostream << entry.first << std::endl;
    ostream << entry.second << std::endl;
  }

  return true;
}

bool Options::read(string const& filename) {
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

bool Options::exists(string& key) const { return options.find(key) != options.end(); }

bool Options::exists(const char* key) const {
  string k(key);
  return exists(k);
}

string Options::get(const char* key, char const* const def) const {
  return (exists(key)) ? options.find(key)->second : def;
}

int Options::getInt(const char* const key, int const def) {
  if (!exists(key)) return def;
  string value = options[key];
  if (!isdigit(value[0])) return 0;
  return stoi(value);
}

float Options::getFloat(const char* const key, float const def) {
  return exists(key) ? stof(options[key]) : def;
}

bool Options::getBool(const char* const key, bool const def) {
  if (!exists(key)) return def;
  string value = options[key];
  if (value.empty()) return def;
  if (tolower(value[0]) == 't' || tolower(value[0]) == 'y') return true;
  return getInt(key) != 0;
}

void Options::set(char const* key, char const* value) { options[string(key)] = string(value); }

void Options::setInt(char const* key, int const value) { options[string(key)] = to_string(value); }

void Options::setFloat(char const* key, float const value) {
  options[string(key)] = to_string(value);
}

void Options::setBool(char const* key, bool const value) {
  options[string(key)] = to_string(value);
}
