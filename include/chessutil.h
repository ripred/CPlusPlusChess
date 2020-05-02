/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one int to represent the piece
 */

#pragma once

namespace chess {

  static int const BOARD_SIZE = 64;
  static unsigned int const White = 1;
  static unsigned int const Black = 0;

  static int const MAX_VALUE = 0x7FFFFFFF;
  static int const MIN_VALUE = 0 - MAX_VALUE;

  static int const values[] = {0, 10000, 30000, 30000, 50000, 90000, MAX_VALUE};

  // Masks
  static unsigned int const Type = 0x07;
  static unsigned int const Unused = 0x88;
  static unsigned int const Side = 0x10;
  static unsigned int const Moved = 0x20;
  static unsigned int const Check = 0x40;

  static unsigned int const Empty = 0;
  static unsigned int const Pawn = 1;
  static unsigned int const Knight = 2;
  static unsigned int const Bishop = 3;
  static unsigned int const Rook = 4;
  static unsigned int const Queen = 5;
  static unsigned int const King = 6;
  static unsigned int const Marker = 7;

  unsigned int getType(unsigned int b);

  bool isEmpty(unsigned int b);

  unsigned int getValue(unsigned int b);

  unsigned int getSide(unsigned int b);

  bool hasMoved(unsigned int b);

  bool inCheck(unsigned int b);

  unsigned int setType(unsigned int b, unsigned int type);

  unsigned int setSide(unsigned int b, unsigned int side);

  unsigned int setMoved(unsigned int b, bool hasMoved);

  unsigned int setCheck(unsigned int b, bool inCheck);

  unsigned int makeSpot(unsigned int type, unsigned int side, bool moved, bool inCheck);
}  // namespace chess
