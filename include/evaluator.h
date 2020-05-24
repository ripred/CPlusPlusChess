//
// evaluator.h
//

#pragma once

#include <board.h>
#include <chessutil.h>

namespace chess {
  class Evaluator {
  public:
    // Filters to decide what to include in our evaluation
    static unsigned int const MATERIAL = 0x01u;
    static unsigned int const CENTER = 0x02u;
    static unsigned int const MOBILE = 0x04u;

    // material values
    static int materialEvaluator(unsigned int p);

    // center location bonus
    static int centerEvaluator(int ndx, unsigned int p);

    static int evaluate(Board const& board);

    static int evaluate(Board const& board, int with);
  };
}  // namespace chess
