//
// board.cpp
// properties and methods for managing a chess board
//

#include <board.h>

#include <algorithm>
#include <string>

using std::toupper;
using std::transform;

using namespace chess;

Board::Board() {
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

bool Board::isEmpty(int const ndx) const { return chess::isEmpty(board[ndx]); }

unsigned int Board::getType(int ndx) const { return chess::getType(board[ndx]); }

unsigned int Board::getSide(int ndx) const { return chess::getSide(board[ndx]); }

bool Board::hasMoved(int ndx) const { return chess::hasMoved(board[ndx]); }

int Board::getValue(int ndx) const { return chess::getValue(board[ndx]); }

bool Board::inCheck(int ndx) const { return chess::inCheck(board[ndx]); }

void Board::setType(int ndx, unsigned int type) { board[ndx] = chess::setType(board[ndx], type); }

void Board::setSide(int ndx, unsigned int side) { board[ndx] = chess::setSide(board[ndx], side); }

void Board::setMoved(int ndx, bool hasMoved) { board[ndx] = chess::setMoved(board[ndx], hasMoved); }

void Board::setCheck(int ndx, bool inCheck) { board[ndx] = chess::setCheck(board[ndx], inCheck); }

vector<string> Board::to_string(Board const& b) {
  char const blackPieces[7] = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};
  char const whitePieces[7] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};

  vector<string> result;
  string line;

  for (auto i = 0; i < BOARD_SIZE; i++) {
    if (i >= 8 && i % 8 == 0) {
      result.push_back(line);
      line.clear();
    }

    auto const type = b.getType(i);
    auto const side = b.getSide(i);

    if (i % 8 == 0) {
      line += std::to_string(8 - (i / 8)) + ' ';
    }
    line += ' ';

    switch (type) {
      case Pawn:
      case Knight:
      case Bishop:
      case Rook:
      case Queen:
      case King:
        line += ((side == White) ? whitePieces[type] : blackPieces[type]);
        break;
      default:
        line += ((((i / 8) + (i % 8)) % 2 == 1) ? ' ' : '.');
        break;
    }
    line += ' ';
  }
  result.push_back(line);
  result.emplace_back("   A  B  C  D  E  F  G  H");

  return result;
}

void Board::generateMoveLists() {
  moves1 = getMovesSorted(turn);
  moves2 = getMovesSorted((turn + 1) % 2);

  // at this point the Side (Black or White) constant is in the .turn value
  // for the board.  Whichever color is to move next, it's moves are in
  // moves1 and the non-moving players moves are in moves2.  Also both
  // pieces1 and pieces2 are updated to contain the pieces with the same
  // logic, pieces1 is for whoever's color is in .turn, the other in pieces2.
}

bool Board::checkDrawByRepetition(Move const& move, int const maxRepetitions) const {
  // Check for draw-by-repetition (same made too many times in a row by a player)
  size_t need = static_cast<int>(pow(2.0, (maxRepetitions + 1)));
  if (history.size() < need) return false;
  int count = 0;
  for (auto i = history.size() - need; i < history.size(); i++) {
    if (history[i] == move) {
      count++;
      if (count >= maxRepetitions) return true;
    }
  }
  return false;
}

bool Board::kingInCheck(unsigned int const side) {
  unsigned int otherSide = (side + 1) % 2;
  vector<Move> opponentMoves = getMoves(otherSide, false);
  // int numMoves = opponentMoves.size();

  for (int ndx = 0; ndx < BOARD_SIZE; ndx++) {
    if (getType(ndx) != King || getSide(ndx) != side) continue;
    for (Move& move : opponentMoves) {
      if (move.getTo() == ndx) {
        return true;
      }
    }
    break;
  }
  return false;
}

void Board::executeMove(Move const& move) {
  int fx = move.getFromCol();
  int fy = move.getFromRow();
  int tx = move.getToCol();
  int ty = move.getToRow();
  int fi = move.getFrom();
  int ti = move.getTo();

  // special check for en passant
  unsigned int type = getType(fi);
  unsigned int toType = getType(ti);
  if (toType == Empty && type == Pawn && fx != tx) {  // en-passant capture
    if (turn != Black)
      taken2.push_back(Pawn);
    else
      taken1.push_back(Pawn);
    board[tx + fy * 8] = makeSpot(Empty, Black, false, false);
  } else {
    if (toType != Empty) {
      if (turn != Black)
        taken2.push_back(toType);
      else
        taken1.push_back(toType);
    }
  }

  board[ti] = board[fi];
  board[fi] = makeSpot(Empty, Black, false, false);
  setMoved(ti, true);

  // See if this is a Castling move:
  if (type == King) {
    int delta = tx - fx;
    if (abs(delta) == 2) {
      int rfi;  // Rook from index
      int rti;  // Rook to index
      if (delta < 0) {
        rfi = fy * 8;      // rook from-index
        rti = fy * 8 + 3;  // rook to-index
      } else {
        rfi = fy * 8 + 7;  // rook from-index
        rti = fy * 8 + 5;  // rook to-index
      }
      board[rti] = board[rfi];
      setMoved(rti, true);
      board[rfi] = makeSpot(Empty, Black, false, false);
    }

    if (getSide(fi) == Black)
      blkKingLoc = ti;
    else
      whtKingLoc = ti;

  } else if (type == Pawn) {
    if (ty == 0 || ty == 7) {
      setType(ti, Queen);
    }
  }

  //  if (numHist >= history.size()) {
  //    numHist -= history.size() / 4;
  //    System.arraycopy(history, history.size() / 4, history, 0, numHist);
  //  }

  history.push_back(move);
  lastMove = move;
}

/**
 * Advance the total number of moves in the game.
 * Also toggle which players turn it is, and generates
 * the following changes in the board:
 *
 *      + .turn  contains the color of the new player to move next
 *      + .turns contains the total number of moves made so far
 *
 *      + move list for current     moving player are in moves1     (turn's Side color: Black or
 * White)
 *      + move list for current non-moving player are in moves2     ( ((turn + 1) % 2)'s Side
 * color: Black or White)
 *      + piece list for current     moving player are in pieces1
 *      + piece list for current non-moving player are in pieces2
 *
 */
void Board::advanceTurn() {
  turns++;
  turn = ((turn + 1) % 2);
  generateMoveLists();
}

vector<Move> Board::getMovesSorted(unsigned int const side) {
  vector<Move> moves = getMoves(side, true);
  sort(moves.begin(), moves.end(),
       [](Move const& m1, Move const& m2) { return m1.getValue() < m2.getValue(); });
  return moves;
}

vector<Move> Board::getMoves(unsigned int const side, bool checkKing) {
  vector<Move> moves;
  vector<Move> pieceMoves;

  // We also update the pieces array and numPieces for this side
  vector<unsigned int> pieces;
  int numPieces = 0;

  for (int ndx = 0; ndx < BOARD_SIZE; ndx++) {
    if (isEmpty(ndx) || getSide(ndx) != side) continue;

    // add piece to list of pieces for this side
    unsigned int b = board[ndx];
    pieces.push_back(b);
    numPieces++;

    int col = ndx % 8;
    int row = ndx / 8;
    switch (getType(ndx)) {
      case Pawn:
        pieceMoves = getPawnMoves(col, row);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        break;
      case Rook:
        pieceMoves = getRookMoves(col, row);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        break;
      case Knight:
        pieceMoves = getKnightMoves(col, row);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        break;
      case Bishop:
        pieceMoves = getBishopMoves(col, row);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        break;
      case Queen:
        pieceMoves = getQueenMoves(col, row);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        break;
      case King:
        pieceMoves = getKingMoves(col, row);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
        break;
    }
  }

  if (side == turn) {
    pieces1 = pieces;
  } else {
    pieces2 = pieces;
  }

  if (checkKing) {
    moves = cleanupMoves(moves, side);
  }

  return moves;
}

vector<Move> Board::cleanupMoves(vector<Move>& moves, unsigned int const side) const {
  vector<Move> valid;
  for (Move const& move : moves) {
    Board current(*this);
    current.executeMove(move);
    if (!current.kingInCheck(side)) {
      valid.push_back(move);
    }
  }
  return valid;
}

bool Board::isValidSpot(int const col, int const row) {
  return (col >= 0) && (col <= 7) && (row >= 0) && (row <= 7);
}

void Board::addMoveIfValid(vector<Move>& moves, int fromCol, int fromRow, int toCol,
                           int toRow) const {
  if (!isValidSpot(fromCol, fromRow)) {
    return;
  }
  if (!isValidSpot(toCol, toRow)) {
    return;
  }
  int fi = fromCol + fromRow * 8;
  int ti = toCol + toRow * 8;

  int value = 0;
  unsigned int pieceType = chess::getType(fi);
  unsigned int pieceSide = chess::getSide(fi);
  if (!chess::isEmpty(ti)) {
    if (chess::getSide(fi) == chess::getSide(ti)) {
      return;
    }
    value = chess::getValue(ti);
  }

  // extra checks if moving a pawn...
  if (pieceType == Pawn) {
    int forward = (pieceSide == Black) ? 1 : -1;
    // if double push
    if (abs(fromRow - toRow) == 2) {
      // not allowed if the pawn has already moved
      if (chess::hasMoved(fi) || !chess::isEmpty(fromCol + (fromRow + forward) * 8)) {
        return;
      }
    }
    // if advancing in same column
    if (fromCol == toCol) {
      // not allowed if a piece is in the way
      if (!chess::isEmpty(ti)) {
        return;
      }
    } else {
      // pawns cannot move diagonally unless
      if (chess::isEmpty(ti)) {
        if (lastMove.getToRow() != (toRow - forward) || lastMove.getToCol() != toCol) {
          return;
        }
        // capturing en passant
        value = chess::getValue(Pawn);
      } else {
        // capturing normal
        value = chess::getValue(ti);
      }
    }
  }

  moves.emplace_back(fromCol, fromRow, toCol, toRow, value);
}

/**
 * Get a list of all possible moves for a pawn at the given location on the board.
 *
 * @param col The column on the board to get moves from
 * @param row The row on the board to get moves from
 * @return A new vector<Move> containing all possible moves a pawn could make from the given
 * spot
 * @throws IllegalArgumentException if the specified location is invalid or if it does not
 * contain a piece to move.
 */
vector<Move> Board::getPawnMoves(int col, int row) const {
  int ndx = col + row * 8;
  int forward = getSide(ndx) == White ? -1 : 1;
  vector<Move> moves;

  addMoveIfValid(moves, col, row, col, row + forward);
  if (!hasMoved(col + row * 8)) {
    addMoveIfValid(moves, col, row, col, row + forward + forward);
  }

  if (isValidSpot(col - 1, row + forward) && !isEmpty((col - 1) + (row + forward) * 8)) {
    addMoveIfValid(moves, col, row, col - 1, row + forward);
  }

  if (isValidSpot(col + 1, row + forward) && !isEmpty((col + 1) + (row + forward) * 8)) {
    addMoveIfValid(moves, col, row, col + 1, row + forward);
  }

  // en-passant! on the left
  int ep1x = col - 1;
  if (isValidSpot(ep1x, row) && getSide(ep1x + row * 8) != getSide(ndx)) {
    if (lastMove.getToCol() == ep1x && lastMove.getToRow() == row) {
      if (abs(lastMove.getFromRow() - lastMove.getToRow()) == 2) {
        if (getType(ep1x + row * 8) == Pawn) {
          addMoveIfValid(moves, col, row, ep1x, row + forward);
        }
      }
    }
  }

  // en-passant! on the right
  ep1x = col + 1;
  if (isValidSpot(ep1x, row) && getSide(ep1x + row * 8) != getSide(ndx)) {
    if (lastMove.getToCol() == ep1x && lastMove.getToRow() == row) {
      if (abs(lastMove.getFromRow() - lastMove.getToRow()) == 2) {
        if (getType(ep1x + row * 8) == Pawn) {
          addMoveIfValid(moves, col, row, ep1x, row + forward);
        }
      }
    }
  }

  return moves;
}

bool Board::addSlider(vector<Move> moves, int col, int row, int x, int y) const {
  if (!isValidSpot(x, y)) {
    return false;
  }
  if (!isEmpty(x + y * 8)) {
    if (getSide(col + row * 8) == getSide(x + y * 8)) return false;
    addMoveIfValid(moves, col, row, x, y);
    return false;
  }
  addMoveIfValid(moves, col, row, x, y);
  return true;
}

/**
 * Get a list of all possible moves for a rook at the given location on the board.
 *
 * @param col The column on the board to get moves from
 * @param row The row on the board to get moves from
 * @return A new vector<Move> containing all possible moves a rook could make from the given
 * spot
 * @throws IllegalArgumentException if the specified location is invalid or if it does not
 * contain a piece to move.
 */
vector<Move> Board::getRookMoves(int col, int row) const {
  vector<Move> moves;

  for (int offset = 1; offset <= 7; offset++) {
    int x = col - offset;
    int y = row;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  for (int offset = 1; offset <= 7; offset++) {
    int x = col + offset;
    int y = row;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  for (int offset = 1; offset <= 7; offset++) {
    int x = col;
    int y = row - offset;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  for (int offset = 1; offset <= 7; offset++) {
    int x = col;
    int y = row + offset;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  return moves;
}

/**
 * Get a list of all possible moves for a knight at the given location on the board.
 *
 * @param col The column on the board to get moves from
 * @param row The row on the board to get moves from
 * @return A new vector<Move> containing all possible moves a knight could make from the given
 * spot
 * @throws IllegalArgumentException if the specified location is invalid or if it does not
 * contain a piece to move.
 */
vector<Move> Board::getKnightMoves(int col, int row) const {
  vector<Move> moves;

  addMoveIfValid(moves, col, row, col - 1, row - 2);
  addMoveIfValid(moves, col, row, col + 1, row - 2);
  addMoveIfValid(moves, col, row, col - 1, row + 2);
  addMoveIfValid(moves, col, row, col + 1, row + 2);
  addMoveIfValid(moves, col, row, col - 2, row - 1);
  addMoveIfValid(moves, col, row, col + 2, row - 1);
  addMoveIfValid(moves, col, row, col - 2, row + 1);
  addMoveIfValid(moves, col, row, col + 2, row + 1);

  return moves;
}

/**
 * Get a list of all possible moves for a bishop at the given location on the board.
 *
 * @param col The column on the board to get moves from
 * @param row The row on the board to get moves from
 * @return A new vector<Move> containing all possible moves a bishop could make from the given
 * spot
 * @throws IllegalArgumentException if the specified location is invalid or if it does not
 * contain a piece to move.
 */
vector<Move> Board::getBishopMoves(int col, int row) const {
  vector<Move> moves;

  for (int offset = 1; offset <= 7; offset++) {
    int x = col - offset;
    int y = row - offset;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  for (int offset = 1; offset <= 7; offset++) {
    int x = col + offset;
    int y = row - offset;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  for (int offset = 1; offset <= 7; offset++) {
    int x = col - offset;
    int y = row + offset;
    if (!addSlider(moves, col, row, x, y)) break;
  }
  for (int offset = 1; offset <= 7; offset++) {
    int x = col + offset;
    int y = row + offset;
    if (!addSlider(moves, col, row, x, y)) break;
  }

  return moves;
}

/**
 * Get a list of all possible moves for a queen at the given location on the board.
 *
 * @param col The column on the board to get moves from
 * @param row The row on the board to get moves from
 * @return A new vector<Move> containing all possible moves a queen could make from the given
 * spot
 * @throws IllegalArgumentException if the specified location is invalid or if it does not
 * contain a piece to move.
 */
vector<Move> Board::getQueenMoves(int col, int row) const {
  vector<Move> moves;
  moves = getRookMoves(col, row);
  vector<Move> bishopMoves = getBishopMoves(col, row);
  moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
  return moves;
}

/**
 * Get a list of all possible moves for a king at the given location on the board.
 *
 * @param col The column on the board to get moves from
 * @param row The row on the board to get moves from
 * @return A new vector<Move> containing all possible moves a king could make from the given
 * spot
 * @throws IllegalArgumentException if the specified location is invalid or if it does not
 * contain a piece to move.
 */
vector<Move> Board::getKingMoves(int col, int row) const {
  int ndx = col + row * 8;
  vector<Move> moves;

  addMoveIfValid(moves, col, row, col - 1, row - 1);
  addMoveIfValid(moves, col, row, col + 1, row - 1);
  addMoveIfValid(moves, col, row, col - 1, row + 1);
  addMoveIfValid(moves, col, row, col + 1, row + 1);
  addMoveIfValid(moves, col, row, col, row - 1);
  addMoveIfValid(moves, col, row, col, row + 1);
  addMoveIfValid(moves, col, row, col - 1, row);
  addMoveIfValid(moves, col, row, col + 1, row);

  // check for king side castling:
  if (!hasMoved(ndx) && isEmpty((col + 1) + row * 8) && isEmpty((col + 2) + row * 8)
      && getType((col + 3) + row * 8) == Rook && !hasMoved((col + 3) + row * 8)) {
    addMoveIfValid(moves, col, row, col + 2, row);
  }

  // check for queen side castling:
  if (!hasMoved(ndx) && isEmpty((col - 1) + row * 8) && isEmpty((col - 2) + row * 8)
      && isEmpty((col - 3) + row * 8) && getType((col - 4) + row * 8) == Rook
      && !hasMoved((col - 4) + row * 8)) {
    addMoveIfValid(moves, col, row, col - 2, row);
  }

  return moves;
}
