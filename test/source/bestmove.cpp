#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <bestmove.h>
#include <board.h>
#include <chessutil.h>
#include <evaluator.h>
#include <minimax.h>
#include <move.h>

#include <iostream>
#include <string>

using namespace chess;
using MoveList = vector<Move>;
using std::find;
using std::get;
using std::tuple;

/**
 * unit tests for computer player move evaluations
 *
 */
TEST_CASE("chess::BestMove") {
  BestMove best(true);
  CHECK(best.value == MIN_VALUE);

  best = BestMove(false);
  CHECK(best.value == MAX_VALUE);

  Move const move(1, 2, 3, 4, 42);
  best = BestMove(move, 7);
  CHECK(best.value == 7);
  CHECK(best.move.getValue() == 7);
  CHECK(best.move.getFrom() == 1 + 2 * 8);
  CHECK(best.move.getTo() == 3 + 4 * 8);
}
