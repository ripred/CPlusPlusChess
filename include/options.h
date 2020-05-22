/**
 * options.h
 *
 * runtime user options
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using std::iostream;
using std::to_string;
using std::string;
using std::find;
using std::map;

namespace chess {
    class Options {
    private:
        map<string const, string> options;

    public:
        Options() = default;
        Options(int const argc, char **argv) {
            parse(argc, argv);
        }

        bool parse(int const argc, char **argv) {
            string lastArg;
            for (auto index = 1; index < argc; ++index) {
                string arg = argv[index];
                int firstChar = arg.empty() ? 0 : arg[0];
                int lastChar = arg.empty() ? 0 : *(arg.end() - 1);
                if (lastChar == '=' || lastChar == ':') {
                    arg = arg.substr(0, arg.size() - 1);
                }
                if (firstChar == '=' || firstChar == ':') {
                    arg = arg.substr(1);
                }
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

        bool write(string const& filename) {
            std::ofstream ostream(filename, std::ios::binary);
            for (auto& entry : options) {
                ostream << entry.first;
                ostream << entry.second;
            }

            return true;
        }

        bool read(string const& filename) {
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

        [[nodiscard]] bool exists(string& key) const {
            return options.find(key) != options.end();
        }

        [[nodiscard]] bool exists(char const * const key) const {
            string k(key);
            return exists(k);
        }

        [[nodiscard]] string get(char const * const key, char const * const def = "") const {
            return (this->exists(key)) ? options.find(key)->second : def;
        }

        [[nodiscard]] int getInt(char const * const key, int const def = 0) {
            if (!exists(key)) return def;
            string value = options[key];
            if (!isdigit(value[0])) return 0;
            return stoi(value);
        }

        [[nodiscard]] double getFloat(char const * const key, double const def = 0.0) {
            return exists(key) ? stof(options[key]) : def;
        }

        [[nodiscard]] bool getBool(char const * const key, bool const def = false) {
            if (!exists(key)) return def;
            string value = options[key];
            if (value.empty()) return def;
            if (tolower(value[0]) == 't' || tolower(value[0]) == 'y') return true;
            return getInt(key) != 0;
        }

        void set(char const * key, char const * value = "") {
            options[string(key)] = string(value);
        }

        void setInt(char const * key, int const value = 0) {
            options[string(key)] = to_string(value);
        }

        void setFloat(char const * key, double const value = 0.0) {
            options[string(key)] = to_string(value);
        }

        void setBool(char const * key, bool const value = true) {
            options[string(key)] = to_string(value);
        }
    };
}
