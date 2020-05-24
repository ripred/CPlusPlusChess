/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one byte to represent the piece
 */

#include "chessutil.h"
namespace chess {
  Piece getType(Piece b) { return Type & b; }

  bool isEmpty(Piece b) { return getType(b) == Empty; }

  int getValue(Piece b) { return values[getType(b)]; }

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

  string getCoords(int index) { return _coords[index]; }

  string getCoords(int file, int rank) { return getCoords(file + rank * 8); }

  string getNotate(int index) { return _notations[index]; }

  string getNotate(int file, int rank) { return getNotate(file + rank * 8); }

  string getName(Piece b) { return _names[getType(b)]; }

  string getColor(Piece b) { return chess::getSide(b) == White ? "White" : "Black"; }

}  // namespace chess
