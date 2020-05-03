#include <board.h>
#include <chessutil.h>
#include <move.h>

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

using namespace chess;

int main(int argc, char** argv) {
  Board board;
  auto lines = board.to_string(board);
  for_each(begin(lines), end(lines), [](auto const& line) { cout << line << endl; });

  return 0;
}
