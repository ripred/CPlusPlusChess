/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one byte to represent the piece
 */

#include "chessutil.h"
namespace chess {
  unsigned int getType(unsigned int b) { return Type & b; }

  bool isEmpty(unsigned int b) { return getType(b) == Empty; }

  int getValue(unsigned int b) { return values[getType(b)]; }

  unsigned int getSide(unsigned int b) { return (Side & b) >> 4; }

  bool hasMoved(unsigned int b) { return (Moved & b) == Moved; }

  bool inCheck(unsigned int b) { return (Check & b) == Check; }

  unsigned int setType(unsigned int b, unsigned int type) { return (b & ~Type) | (type & Type); }

  unsigned int setSide(unsigned int b, unsigned int side) {
    return (b & ~Side) | ((side << 4) & Side);
  }

  unsigned int setMoved(unsigned int b, bool hasMoved) {
    return (b & ~Moved) | (hasMoved ? Moved : 0);
  }

  unsigned int setCheck(unsigned int b, bool inCheck) {
    return (b & ~Check) | (inCheck ? Check : 0);
  }

  unsigned int makeSpot(unsigned int type, unsigned int side, bool moved, bool inCheck) {
    unsigned int b = 0;
    b = setType(b, type);
    b = setSide(b, side);
    b = setMoved(b, moved);
    b = setCheck(b, inCheck);
    return b;
  }

  string getCoords(int index) { return coords[index]; }

  string getCoords(int file, int rank) { return getCoords(file + rank * 8); }

  string getNotate(int index) { return notations[index]; }

  string getNotate(int file, int rank) { return getNotate(file + rank * 8); }

  string getName(unsigned int b) { return names[getType(b)]; }

  string getColor(unsigned int b) { return chess::getSide(b) == White ? "White" : "Black"; }

}  // namespace chess
