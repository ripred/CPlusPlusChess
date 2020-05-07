//
// minimax.h
//
// minimax algorithm implemented in c++ for chess board evaluation
//

#pragma once

#include <bestmove.h>
#include <board.h>
#include <chessutil.h>
#include <move.h>

using namespace chess;

class Minimax {
public:
  int maxDepth;
  BestMove best;
  bool maximize;
  long movesProcessed;

  explicit Minimax(int max_depth)
      : maxDepth(max_depth), best(true), maximize(true), movesProcessed(0L) {}

  Move bestMove(Board& board) {
    unsigned int const side = board.turn;
    maximize = (side == White);
    movesProcessed = 0L;

    best = BestMove(maximize);

    return best.move;
  }
};
