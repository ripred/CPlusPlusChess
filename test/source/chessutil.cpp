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
 * unit tests for low level chessutil functions
 *
 */
TEST_CASE("chess::chessutil") {
  int const whitePawn = makeSpot(Pawn, White, false, false);
  int const blackPawn = makeSpot(Pawn, Black, false, false);
  int const movedWhitePawn = setCheck(setMoved(whitePawn, true), true);
  int const movedBlackPawn = setCheck(setMoved(blackPawn, true), true);

  CHECK(getType(whitePawn) == Pawn);
  CHECK(getType(blackPawn) == Pawn);
  CHECK(getType(Rook) == Rook);
  CHECK(getType(Knight) == Knight);
  CHECK(getType(Bishop) == Bishop);
  CHECK(getType(Queen) == Queen);
  CHECK(getType(King) == King);

  CHECK(isEmpty(0) == true);
  CHECK(isEmpty(Empty) == true);
  CHECK(isEmpty(whitePawn) == false);
  CHECK(isEmpty(blackPawn) == false);
  CHECK(isEmpty(Rook) == false);
  CHECK(isEmpty(Knight) == false);
  CHECK(isEmpty(Bishop) == false);
  CHECK(isEmpty(Queen) == false);
  CHECK(isEmpty(King) == false);

  CHECK(getValue(whitePawn) == values[Pawn]);
  CHECK(getValue(blackPawn) == values[Pawn]);
  CHECK(getValue(Rook) == values[Rook]);
  CHECK(getValue(Knight) == values[Knight]);
  CHECK(getValue(Bishop) == values[Bishop]);
  CHECK(getValue(Queen) == values[Queen]);
  CHECK(getValue(King) == values[King]);

  CHECK(getSide(whitePawn) == 1);
  CHECK(getSide(blackPawn) == 0);

  CHECK(hasMoved(whitePawn) == false);
  CHECK(hasMoved(blackPawn) == false);
  CHECK(hasMoved(movedWhitePawn) == true);
  CHECK(hasMoved(movedBlackPawn) == true);

  CHECK(inCheck(whitePawn) == false);
  CHECK(inCheck(blackPawn) == false);
  CHECK(inCheck(movedWhitePawn) == true);
  CHECK(inCheck(movedBlackPawn) == true);

  CHECK(setType(Empty, Pawn) == getType(whitePawn));
  CHECK(setType(Empty, Pawn) == getType(blackPawn));

  CHECK(getSide(setSide(setType(Empty, Pawn), White)) == White);
  CHECK(getSide(setSide(setType(Empty, Pawn), Black)) == Black);

  CHECK(setMoved(setSide(setType(Empty, Pawn), White), true) == (Moved | Pawn | Side));
  CHECK(setMoved(setSide(setType(Empty, Pawn), Black), true) == (Moved | Pawn));

  CHECK(getCoords(44) == string("4,5"));
  CHECK(getCoords(4, 5) == string("4,5"));
  CHECK(getNotate(4, 5) == string("e3"));
  CHECK(getNotate(44) == string("e3"));
  CHECK(getName(Pawn) == string("Pawn"));
  CHECK(getName(Knight) == string("Knight"));
  CHECK(getName(Bishop) == string("Bishop"));
  CHECK(getName(Rook) == string("Rook"));
  CHECK(getName(Queen) == string("Queen"));
  CHECK(getName(King) == string("King"));
  CHECK(getColor(whitePawn) == string("White"));
  CHECK(getColor(blackPawn) == string("Black"));
}
