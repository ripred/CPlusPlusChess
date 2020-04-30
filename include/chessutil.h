/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one int to represent the piece
 */

#pragma once

namespace chess {

  static int const BOARD_SIZE = 64;
  static int const White = 1;
  static int const Black = 0;

  static int const MAX_VALUE = 0x7FFFFFFF;
  static int const MIN_VALUE = 0 - MAX_VALUE;

  static int const values[] = {0, 10000, 30000, 30000, 50000, 90000, MAX_VALUE};

  // Masks
  static int const Type = 0x07;
  static int const Unused = 0x88;
  static int const Side = 0x10;
  static int const Moved = 0x20;
  static int const Check = 0x40;

  static int const Empty = 0;
  static int const Pawn = 1;
  static int const Knight = 2;
  static int const Bishop = 3;
  static int const Rook = 4;
  static int const Queen = 5;
  static int const King = 6;
  static int const Marker = 7;

  int getType(int b);

  bool isEmpty(int b);

  int getValue(int b);

  int getSide(int b);

  bool hasMoved(int b);

  bool inCheck(int b);

  int setType(int b, int type);

  int setSide(int b, int side);

  int setMoved(int b, bool hasMoved);

  int setCheck(int b, bool inCheck);

  int makeSpot(int type, int side, bool moved, bool inCheck);
}  // namespace chess
