//
// bestmove.h
//
// the BestMove class holds the best move found for a given board and player
//

#pragma once

#include <board.h>
#include <chessutil.h>
#include <move.h>

using namespace chess;

class BestMove {
public:
  Move move;
  int value;
  int movesExamined;

  explicit BestMove(bool maximize) : movesExamined(0) {
    value = maximize ? chess::MIN_VALUE : chess::MAX_VALUE;
  }
};
