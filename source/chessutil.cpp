/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one byte to represent the piece
 */

#include <chessutil.h>

namespace chess {
  Piece getType(Piece b) { return Type & b; }

  bool isEmpty(Piece b) { return getType(b) == Empty; }

  int getValue(Piece b) { return pieceValues[getType(b)]; }

  Piece getSide(Piece b) { return (Side & b) >> 4u; }

  bool hasMoved(Piece b) { return (Moved & b) == Moved; }

  bool inCheck(Piece b) { return (Check & b) == Check; }

  bool isPromoted(Piece b) { return (Promoted & b) == Promoted; }

  Piece setType(Piece b, Piece type) { return (b & ~Type) | (type & Type); }

  Piece setSide(Piece b, Piece side) { return (b & ~Side) | ((side << 4u) & Side); }

  Piece setMoved(Piece b, bool hasMoved) { return (b & ~Moved) | (hasMoved ? Moved : 0); }

  Piece setCheck(Piece b, bool inCheck) { return (b & ~Check) | (inCheck ? Check : 0); }

  Piece setPromoted(Piece b, bool promoted) { return (b & ~Promoted) | (promoted ? Promoted : 0); }

  Piece makeSpot(Piece type, Piece side, bool moved, bool inCheck) {
    Piece b = 0;
    b = setType(b, type);
    b = setSide(b, side);
    b = setMoved(b, moved);
    b = setCheck(b, inCheck);
    return b;
  }

  string getCoords(int index) {
    static const array<string const, BOARD_SIZE> coords = {
        "0,0", "1,0", "2,0", "3,0", "4,0", "5,0", "6,0", "7,0", "0,1", "1,1", "2,1", "3,1", "4,1",
        "5,1", "6,1", "7,1", "0,2", "1,2", "2,2", "3,2", "4,2", "5,2", "6,2", "7,2", "0,3", "1,3",
        "2,3", "3,3", "4,3", "5,3", "6,3", "7,3", "0,4", "1,4", "2,4", "3,4", "4,4", "5,4", "6,4",
        "7,4", "0,5", "1,5", "2,5", "3,5", "4,5", "5,5", "6,5", "7,5", "0,6", "1,6", "2,6", "3,6",
        "4,6", "5,6", "6,6", "7,6", "0,7", "1,7", "2,7", "3,7", "4,7", "5,7", "6,7", "7,7"};
    return coords[index];
  }

  string getCoords(int file, int rank) { return getCoords(file + rank * 8); }

  string getNotate(int index) {
    static const array<string const, BOARD_SIZE> notations
        = {"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7", "c7", "d7", "e7",
           "f7", "g7", "h7", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a5", "b5",
           "c5", "d5", "e5", "f5", "g5", "h5", "a4", "b4", "c4", "d4", "e4", "f4", "g4",
           "h4", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a2", "b2", "c2", "d2",
           "e2", "f2", "g2", "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};
    return notations[index];
  }

  string getNotate(int file, int rank) { return getNotate(file + rank * 8); }

  string getName(Piece b) {
    static const array<string const, 7> names
        = {"Empty", "Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};
    return names[getType(b)];
  }

  string getColor(Piece b) { return chess::getSide(b) == White ? "White" : "Black"; }

  ScopedLock::ScopedLock(mutex &m) : ref(m) { ref.lock(); }
  ScopedLock::~ScopedLock() { ref.unlock(); }

  string addCommas(long int value) {
    string str = to_string(value);
    size_t numDigits = str.size();
    if (numDigits < 4u) return str;

    string withCommas;
    size_t insertWhen0 = numDigits % 3u;
    // if insertWhen0 starts at 0 we could put a leading comma before any digits so reset before we
    // start
    if (insertWhen0 == 0) insertWhen0 = 3u;
    for (auto it = begin(str); it != end(str); it++) {
      if (insertWhen0 == 0) {
        withCommas += ',';
        insertWhen0 = 3u;
      }
      insertWhen0--;
      withCommas.insert(end(withCommas), it, it + 1);
    }
    return withCommas;
  }

}  // namespace chess
