//
// evaluator.cpp
//

#include <evaluator.h>

namespace chess {
  /// Material values
  /// gives ranked value bonus points for all
  /// remaining pieces
  int Evaluator::materialEvaluator(Piece p) {
    return getValue(p) * ((getSide(p) == Black) ? -1 : 1);
  }

  /// Center location bonus
  /// gives more points for moves that
  /// occupy the center of the board
  int Evaluator::centerEvaluator(int location, Piece piece) {
    Piece type = getType(piece);
    if (type == King)
      return 0;  // let's not encourage the king to wander to the board center mmkay?

    int dx = location % 8;
    if (dx > 3) dx = 7 - dx;
    int dy = location / 8;
    if (dy > 3) dy = 7 - dy;

    return static_cast<int>((dx + dy) * type) * ((getSide(piece) == Black) ? -1 : 1);
  }

  /// Evaluate the identity (score) of the board state.
  /// Positive scores indicate an advantage for white and
  /// Negative scores indicate an advantage for black.
  int Evaluator::evaluate(Board const& board,
                          unsigned filter /* = MATERIAL | CENTER | MOBILITY */) {
    int score = 0;

    int mobilityBonus = 3;
    int centerBonus = 5;

    for (int ndx = 0; ndx < BOARD_SIZE; ndx++) {
      Piece p = board.board[ndx];

      /// The score or 'identity property' of the board
      /// includes points for all pieces the player has
      /// remaining.
      if (filter & MATERIAL) {
        score += materialEvaluator(p);
      }

      /// The score or 'identity property' of the board
      /// includes points for how close the remaining pieces
      /// are to the center of the board
      if (filter & CENTER) {
        score += centerEvaluator(ndx, p) * centerBonus;
      }
    }

    /// The score or 'identity property' of the board
    /// includes extra points for how many totals moves
    /// (mobility) the remaining pieces can make.
    if (filter & MOBILITY) {
      int factor = (board.turn == Black) ? -1 : 1;

      score += static_cast<int>(board.moves1.size()) * mobilityBonus * factor;
      score -= static_cast<int>(board.moves2.size()) * mobilityBonus * factor;
    }
    return score;
  }
}  // namespace chess
