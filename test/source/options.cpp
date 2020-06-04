#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <options.h>

using namespace chess;

/**
 * unit tests for Options objects
 *
 */
TEST_CASE("chess::Options") {
  Options options;

  CHECK(!options.exists("nonexist"));
  CHECK(!options.getBool("nonexist"));
  CHECK(options.getInt("nonexist") == 0);
  CHECK(options.get("nonexist") == "");

  char* argv[] = {(char*)"float_val",   (char*)":",
                  (char*)"123.789",

                  (char*)"int_val",     (char*)"=",
                  (char*)"123456",

                  (char*)"bool_val",    (char*)"=",
                  (char*)"true",

                  (char*)"string_val:", (char*)"string_value",
                  (char*)"trailing_val"};
  options = Options(sizeof(argv) / sizeof(*argv), argv);

  CHECK(options.get("string_val") == "string_value");
  CHECK(options.getInt("int_val") == 123456);
  CHECK((float)(int(options.getFloat("float_val") * 1000.0f) / 1000.0f) == 123.78901f);
  CHECK(options.getBool("bool_val") /* == true*/);
  CHECK(options.exists("trailing_val") /* == true*/);

  options.write("options.txt");
  options.clear();
  options.read("options.txt");
  CHECK(options.get("string_val") == "string_value");
  CHECK(options.getInt("int_val") == 123456);
  CHECK((float)(int(options.getFloat("float_val") * 1000.0f) / 1000.0f) == 123.78901f);
  CHECK(options.getBool("bool_val") /* == true*/);

  options.clear();
  options.set("test", "string");
  CHECK(options.get("test") == "string");

  options.clear();
  CHECK(!options.getBool("useCache"));
  options.set("useCache");
  CHECK(options.getBool("useCache", true));
  options.setBool("useCache", false);
  CHECK(!options.getBool("useCache"));
  options.setBool("useCache");
  CHECK(options.getBool("useCache"));

  options.clear();
  CHECK(options.getInt("test") == 0);
  options.setInt("test", 123);
  CHECK(options.getInt("test") == 123);

  options.clear();
  CHECK(options.getFloat("test") == 0.0);
  options.setFloat("test", 123.456f);
  CHECK(options.getFloat("test") == 123.456f);
}
