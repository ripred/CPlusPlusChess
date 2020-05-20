#include <doctest/doctest.h>
#include <pthread.h>

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
 * unit tests for Move objects
 *
 */
TEST_CASE("chess::Move") {
  Move move1(1, 2, 3, 4, 1000);

  CHECK(move1.getFromCol() == 1);
  CHECK(move1.getFromRow() == 2);
  CHECK(move1.getToCol() == 3);
  CHECK(move1.getToRow() == 4);

  CHECK(move1.getFrom() == 1 + 2 * 8);
  CHECK(move1.getTo() == 3 + 4 * 8);

  CHECK(move1.getValue() == 1000);

  move1.setValue(2000);
  CHECK(move1.getValue() == 2000);

  Move move2(move1);
  CHECK(move1.getFromCol() == move2.getFromCol());
  CHECK(move1.getFromRow() == move2.getFromRow());
  CHECK(move1.getToCol() == move2.getToCol());
  CHECK(move1.getToRow() == move2.getToRow());
  CHECK(move1.getFrom() == move2.getFrom());
  CHECK(move1.getTo() == move2.getTo());
  CHECK(move1.getValue() == move2.getValue());

  CHECK((move1 == move1) == true);
  CHECK((move1 == move2) == true);

  string correct = "1,2 to 3,4 b6 to d4 value:2000";
  CHECK(move1.to_string() == correct);

  Board game;
  move1 = Move();
  CHECK(move1.isValid(game) == false);
  move1 = Move(4, 1, 4, 2, 0);
  CHECK(move1.isValid(game) == true);
  CHECK(move1.isValid() == true);
}
