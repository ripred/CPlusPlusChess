//
// Board.h
// properties and methods for managing a chess board
//

#pragma once

#include <vector>
using std::vector;

#include <chessutil.h>
#include <move.h>

namespace chess {
  class Board {
  private:
  public:
    unsigned int board[BOARD_SIZE];
    vector<Move> moves1;
    int numMoves1;
    vector<Move> moves2;
    int numMoves2;
    vector<Move> history;
    int numHist;
    vector<unsigned int> taken1;
    int numTaken1;
    vector<unsigned int> taken2;
    int numTaken2;
    vector<unsigned int> pieces1;
    int numPieces1;
    vector<unsigned int> pieces2;
    int numPieces2;
    Move lastMove;

    int maxRep;
    int turns;
    int turn;
    int blkKingLoc;
    int whtKingLoc;

  public:
    Board();
    Board(Board const& ref);

    bool isEmpty(int ndx) const;
    unsigned int getType(int ndx) const;
    unsigned int getSide(int ndx) const;
    bool hasMoved(int ndx) const;
    int getValue(int ndx) const;
    bool inCheck(int ndx) const;
    void setType(int ndx, unsigned int type);
    void setSide(int ndx, unsigned int side);
    void setMoved(int ndx, bool hasMoved);
    void setCheck(int ndx, bool inCheck);
    vector<string> to_string(Board const& b) const;
  };

}  // namespace chess
