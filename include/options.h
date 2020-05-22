/**
 * options.h
 *
 * runtime user options
 *
 */

#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

using std::find;
using std::iostream;
using std::map;
using std::string;
using std::to_string;

namespace chess {
  class Options {
  private:
    map<string const, string> options;

  public:
    Options() = default;
    Options(int argc, char** argv);

    void clear();
    bool parse(int argc, char** argv);
    bool write(string const& filename);
    bool read(string const& filename);

    [[nodiscard]] bool exists(string& key) const;
    [[nodiscard]] bool exists(const char* key) const;
    [[nodiscard]] string get(const char* key, char const* def = "") const;
    [[nodiscard]] int getInt(const char* key, int def = 0);
    [[nodiscard]] float getFloat(const char* key, float def = 0.0);
    [[nodiscard]] bool getBool(const char* key, bool def = false);
    void set(char const* key, char const* value = "");
    void setInt(char const* key, int value = 0);
    void setFloat(char const* key, float value = 0.0);
    void setBool(char const* key, bool value = true);
  };
}  // namespace chess
