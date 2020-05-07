#include <bestmove.h>

using namespace chess;

BestMove::BestMove(bool maximize) : movesExamined(0) {
  value = maximize ? chess::MIN_VALUE : chess::MAX_VALUE;
}

BestMove::BestMove(Move const& m, int val) : move(m), value(val) { move.setValue(val); }
