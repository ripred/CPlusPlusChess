#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <board.h>
#include <chessutil.h>

TEST_CASE("chess::chessutil") {
  using namespace chess;

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
}

TEST_CASE("chess::Board") {
  using namespace chess;

  // test default constrcutor
  Board game;

  CHECK(game.board[0] == Rook);

  CHECK(isEmpty(game.board[0]) == false);
  CHECK(isEmpty(game.board[17]) == true);

  CHECK(getValue(game.board[0]) == values[Rook]);
  CHECK(getValue(game.board[1]) == values[Knight]);
  CHECK(getValue(game.board[2]) == values[Bishop]);
  CHECK(getValue(game.board[3]) == values[Queen]);
  CHECK(getValue(game.board[4]) == values[King]);
  CHECK(getValue(game.board[8]) == values[Pawn]);
  CHECK(getValue(game.board[63]) == values[Rook]);
  CHECK(getSide(game.board[0]) == Black);
  CHECK(getSide(game.board[63]) == White);
  CHECK(hasMoved(game.board[0]) == false);
  CHECK(inCheck(game.board[0]) == false);

  // test copy constrcutor
  Board game2(game);
  CHECK(isEmpty(game2.board[0]) == false);
  CHECK(isEmpty(game2.board[17]) == true);
}
