//
// board.cpp
// properties and methods for managing a chess board
//

#include <board.h>

using namespace chess;

Board::Board() : board{}, lastMove{0, 0, 0, 0, 0} {
  board[0 + 0 * 8] = makeSpot(Rook, Black, false, false);
  board[1 + 0 * 8] = makeSpot(Knight, Black, false, false);
  board[2 + 0 * 8] = makeSpot(Bishop, Black, false, false);
  board[3 + 0 * 8] = makeSpot(Queen, Black, false, false);
  board[4 + 0 * 8] = makeSpot(King, Black, false, false);
  board[5 + 0 * 8] = makeSpot(Bishop, Black, false, false);
  board[6 + 0 * 8] = makeSpot(Knight, Black, false, false);
  board[7 + 0 * 8] = makeSpot(Rook, Black, false, false);
  board[0 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[1 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[2 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[3 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[4 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[5 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[6 + 1 * 8] = makeSpot(Pawn, Black, false, false);
  board[7 + 1 * 8] = makeSpot(Pawn, Black, false, false);

  board[0 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[1 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[2 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[3 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[4 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[5 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[6 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[7 + 6 * 8] = makeSpot(Pawn, White, false, false);
  board[0 + 7 * 8] = makeSpot(Rook, White, false, false);
  board[1 + 7 * 8] = makeSpot(Knight, White, false, false);
  board[2 + 7 * 8] = makeSpot(Bishop, White, false, false);
  board[3 + 7 * 8] = makeSpot(Queen, White, false, false);
  board[4 + 7 * 8] = makeSpot(King, White, false, false);
  board[5 + 7 * 8] = makeSpot(Bishop, White, false, false);
  board[6 + 7 * 8] = makeSpot(Knight, White, false, false);
  board[7 + 7 * 8] = makeSpot(Rook, White, false, false);
}

Board::Board(Board const& ref) : board{}, lastMove{0, 0, 0, 0, 0} {
  for (int ndx = 0; ndx < BOARD_SIZE; ndx++) {
    board[ndx] = ref.board[ndx];
  }
}

bool Board::isEmpty(int ndx) const { return chess::isEmpty(board[ndx]); }

unsigned int Board::getType(int ndx) const { return chess::getType(board[ndx]); }

unsigned int Board::getSide(int ndx) const { return chess::getSide(board[ndx]); }

bool Board::hasMoved(int ndx) const { return chess::hasMoved(board[ndx]); }

int Board::getValue(int ndx) const { return chess::getValue(board[ndx]); }

bool Board::inCheck(int ndx) const { return chess::inCheck(board[ndx]); }

void Board::setType(int ndx, unsigned int type) { board[ndx] = chess::setType(board[ndx], type); }

void Board::setSide(int ndx, unsigned int side) { board[ndx] = chess::setSide(board[ndx], side); }

void Board::setMoved(int ndx, bool hasMoved) { board[ndx] = chess::setMoved(board[ndx], hasMoved); }

void Board::setCheck(int ndx, bool inCheck) { board[ndx] = chess::setCheck(board[ndx], inCheck); }
