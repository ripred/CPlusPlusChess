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
    static unsigned int const MATERIAL = 0x01;
    static unsigned int const CENTER = 0x02;
    static unsigned int const MOBILE = 0x04;

    // a material values plugin:
    static int materialEvaluator(unsigned int p) {
      return getValue(p) * ((getSide(p) == Black) ? -1 : 1);
    }

    // a center location bonus plugin:
    static int centerEvaluator(int ndx, unsigned int p) {
      unsigned int type = getType(p);
      if (type == King) return 0;

      int dx = ndx % 8;
      if (dx > 3) dx = 7 - dx;
      int dy = ndx / 8;
      if (dy > 3) dy = 7 - dy;

      return (dx + dy) * type * ((getSide(p) == Black) ? -1 : 1);
    }

    static int evaluate(Board const& board) { return evaluate(board, MATERIAL | CENTER | MOBILE); }

    static int evaluate(Board const& board, int with) {
      int score = 0;

      int mobilityBonus = 3;
      int centerBonus = 5;

      for (unsigned int ndx = 0; ndx < BOARD_SIZE; ndx++) {
        unsigned int p = board.board[ndx];

        if (with & MATERIAL) {
          score += materialEvaluator(p);
        }

        if (with & CENTER) {
          score += centerEvaluator(ndx, p) * centerBonus;
        }

        if (with & MOBILE) {
          int factor = (board.turn == Black) ? -1 : 1;

          score += static_cast<int>(board.moves1.size()) * mobilityBonus * factor;
          score -= static_cast<int>(board.moves2.size()) * mobilityBonus * factor;
        }
      }
      return score;
    }
  };
}  // namespace chess
