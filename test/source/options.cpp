#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <chessutil.h>
#include <options.h>

#include <string>

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

  char* argv[]
      = {(char*)"float_val:", (char*)"123.789", (char*)"int_val:",    (char*)"123456",
         (char*)"bool_val:",  (char*)"true",    (char*)"string_val:", (char*)"string_value"};
  options = Options(sizeof(argv) / sizeof(*argv), argv);

  CHECK(options.get("string_val") == "string_value");
  CHECK(options.getInt("int_val") == 123456);
  CHECK((float)(int(options.getFloat("float_val") * 1000.0f) / 1000.0f) == 123.789f);
  CHECK(options.getBool("bool_val") /* == true*/);

  options = Options();
  options.set("test", "string");
  CHECK(options.get("test") == "string");

  options = Options();
  CHECK(!options.getBool("useCache"));
  options.set("useCache");
  CHECK(options.getBool("useCache", true));
  options.setBool("useCache", false);
  CHECK(!options.getBool("useCache"));
  options.setBool("useCache");
  CHECK(options.getBool("useCache"));

  options = Options();
  CHECK(options.getInt("test") == 0);
  options.setInt("test", 123);
  CHECK(options.getInt("test") == 123);

  options = Options();
  CHECK(options.getFloat("test") == 0.0);
  options.setFloat("test", 123.456f);
  CHECK(options.getFloat("test") == 123.456f);
}
