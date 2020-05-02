#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

#include <board.h>
#include <chessutil.h>
#include <move.h>

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

  // test default constructor
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

  // test copy constructor
  Board game2(game);
  CHECK(isEmpty(game2.board[0]) == false);
  CHECK(isEmpty(game2.board[17]) == true);

  // test chessutil fascade
  CHECK(game.isEmpty(0) == false);
  CHECK(game.isEmpty(17) == true);
  CHECK(game.getValue(0) == values[Rook]);
  CHECK(game.getValue(1) == values[Knight]);
  CHECK(game.getValue(2) == values[Bishop]);
  CHECK(game.getValue(3) == values[Queen]);
  CHECK(game.getValue(4) == values[King]);
  CHECK(game.getValue(8) == values[Pawn]);
  CHECK(game.getValue(63) == values[Rook]);
  CHECK(game.getSide(0) == Black);
  CHECK(game.getSide(63) == White);
  CHECK(game.hasMoved(0) == false);
  CHECK(game.inCheck(0) == false);
}

TEST_CASE("chess::Move") {
  using namespace chess;

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

  string correct = "[Move from 1,2 to 3,4 (b6 to d4) value:2000]";
  CHECK(move1.to_string() == correct);
}
