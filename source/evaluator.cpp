//
// evaluator.cpp
//

#include <evaluator.h>

using namespace chess;

// material values
int Evaluator::materialEvaluator(unsigned int p) {
  return getValue(p) * ((getSide(p) == Black) ? -1 : 1);
}

// center location bonus
int Evaluator::centerEvaluator(int ndx, unsigned int p) {
  unsigned int type = getType(p);
  if (type == King) return 0;

  int dx = ndx % 8;
  if (dx > 3) dx = 7 - dx;
  int dy = ndx / 8;
  if (dy > 3) dy = 7 - dy;

  return (dx + dy) * type * ((getSide(p) == Black) ? -1 : 1);
}

int Evaluator::evaluate(Board const& board) { return evaluate(board, MATERIAL | CENTER | MOBILE); }

int Evaluator::evaluate(Board const& board, int filter) {
  int score = 0;

  int mobilityBonus = 3;
  int centerBonus = 5;

  for (unsigned int ndx = 0; ndx < BOARD_SIZE; ndx++) {
    unsigned int p = board.board[ndx];

    if (filter & MATERIAL) {
      score += materialEvaluator(p);
    }

    if (filter & CENTER) {
      score += centerEvaluator(ndx, p) * centerBonus;
    }

    if (filter & MOBILE) {
      int factor = (board.turn == Black) ? -1 : 1;

      score += static_cast<int>(board.moves1.size()) * mobilityBonus * factor;
      score -= static_cast<int>(board.moves2.size()) * mobilityBonus * factor;
    }
  }
  return score;
}
