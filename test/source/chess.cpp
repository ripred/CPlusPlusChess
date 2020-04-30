#include <doctest/doctest.h>
#include <Board.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#  include <sstream>
#endif

TEST_CASE("Chess") {
using namespace chess;

Board game;

CHECK(game.board[0] == 4);

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
}
