//
// bestmove.h
//
// the BestMove class holds the best move found for a given board and player
//

#pragma once

#include <board.h>
#include <chessutil.h>
#include <move.h>

namespace chess {
  class BestMove {
  public:
    Move move;
    int value;
    int movesExamined;

    explicit BestMove(bool maximize);
    BestMove(Move const& m, int val);
  };
}  // namespace chess
