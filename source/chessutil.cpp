/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one byte to represent the piece
 */

#include "chessutil.h"

namespace chess {

  int getType(int b) {
      return Type & b;
  }

  bool isEmpty(int b) {
      return getType(b) == Empty;
  }

  int getValue(int b) {
      return values[getType(b)];
  }

  int getSide(int b) {
      return (Side & b) >> 4;
  }

  bool hasMoved(int b) {
      return (Moved & b) == Moved;
  }

  bool inCheck(int b) {
      return (Check & b) == Check;
  }

  int setType(int b, int type) {
      return (b & ~Type) | (type & Type);
  }

  int setSide(int b, int side) {
      return (b & ~Side) | ((side << 4) & Side);
  }

  int setMoved(int b, bool hasMoved) {
      return (b & ~Moved) | (hasMoved ? Moved : 0);
  }

  int setCheck(int b, bool inCheck) {
      return (b & ~Check) | (inCheck ? Check : 0);
  }

  int makeSpot(int type, int side, bool moved, bool inCheck) {
      int b = 0;
      b = setType(b, type);
      b = setSide(b, side);
      b = setMoved(b, moved);
      b = setCheck(b, inCheck);
      return b;
  }

}  // namespace chess
