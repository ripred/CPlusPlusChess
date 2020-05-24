#include <bestmove.h>
#include <chessutil.h>

namespace chess {
  BestMove::BestMove(bool maximize) : movesExamined(0) { value = maximize ? MIN_VALUE : MAX_VALUE; }

  BestMove::BestMove(Move const& m, int val) : move(m), value(val) { move.setValue(val); }
}  // namespace chess
