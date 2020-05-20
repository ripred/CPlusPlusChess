/**
 * The chessutil namespace is used to interpret and/or set the attributes
 * of a chess piece using one int to represent the piece
 */

#pragma once

#include <map>
using std::map;

#include <limits>
using std::numeric_limits;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

namespace chess {

  typedef unsigned int Bits;
  typedef vector<Bits> PieceList;
  typedef map<Bits, PieceList> SidePieceMap;
  typedef map<Bits, SidePieceMap> PieceMap;

  static string const notations[64]{
      "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7", "c7", "d7", "e7",
      "f7", "g7", "h7", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a5", "b5",
      "c5", "d5", "e5", "f5", "g5", "h5", "a4", "b4", "c4", "d4", "e4", "f4", "g4",
      "h4", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a2", "b2", "c2", "d2",
      "e2", "f2", "g2", "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

  static string const coords[64]{
      "0,0", "1,0", "2,0", "3,0", "4,0", "5,0", "6,0", "7,0", "0,1", "1,1", "2,1", "3,1", "4,1",
      "5,1", "6,1", "7,1", "0,2", "1,2", "2,2", "3,2", "4,2", "5,2", "6,2", "7,2", "0,3", "1,3",
      "2,3", "3,3", "4,3", "5,3", "6,3", "7,3", "0,4", "1,4", "2,4", "3,4", "4,4", "5,4", "6,4",
      "7,4", "0,5", "1,5", "2,5", "3,5", "4,5", "5,5", "6,5", "7,5", "0,6", "1,6", "2,6", "3,6",
      "4,6", "5,6", "6,6", "7,6", "0,7", "1,7", "2,7", "3,7", "4,7", "5,7", "6,7", "7,7"};

  static string const names[7]{"Empty", "Pawn", "Knight", "Bishop", "Rook", "Queen", "King"};

  static int const BOARD_SIZE = 64;
  static unsigned int const White = 1;
  static unsigned int const Black = 0;

  static int const MAX_VALUE = numeric_limits<signed int>::max() / 2;
  static int const MIN_VALUE = 0 - MAX_VALUE;

  static int const values[7] = {0, 10000, 30000, 30000, 50000, 90000, MAX_VALUE};

  // Masks
  static unsigned int const Type = 0x07;
  // static unsigned int const Unused = 0x88;
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
  // static unsigned int const Marker = 7;

  unsigned int getType(unsigned int b);

  bool isEmpty(unsigned int b);

  int getValue(unsigned int b);

  unsigned int getSide(unsigned int b);

  bool hasMoved(unsigned int b);

  bool inCheck(unsigned int b);

  unsigned int setType(unsigned int b, unsigned int type);

  unsigned int setSide(unsigned int b, unsigned int side);

  unsigned int setMoved(unsigned int b, bool hasMoved);

  unsigned int setCheck(unsigned int b, bool inCheck);

  unsigned int makeSpot(unsigned int type, unsigned int side, bool moved = false,
                        bool inCheck = false);

  // common strings
  string getCoords(int index);
  string getCoords(int file, int rank);
  string getNotate(int index);
  string getNotate(int file, int rank);
  string getName(unsigned int b);
  string getColor(unsigned int b);

#define UNUSED(a) \
  do {            \
    (void)(a);    \
  } while (0);

}  // namespace chess
