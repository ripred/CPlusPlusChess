//
// evaluator.cpp
//

#include <evaluator.h>

namespace chess {
    /// Material values
    /// gives ranked value bonus points for all remaining pieces
    int Evaluator::materialEvaluator(Piece p) { return getValue(p) / 100; }

    /// Center location bonus
    /// gives more points for moves that are closer the center of the board
    int Evaluator::centerEvaluator(unsigned int location, Piece piece) {
        Piece type = getType(piece);
        if (type == King)
            return 0;  // let's not encourage the king to wander to the board center mmkay?

        unsigned int dx = location % 8;
        if (dx > 3) dx = 7 - dx;
        unsigned int dy = location / 8;
        if (dy > 3) dy = 7 - dy;

        return static_cast<int>((dx + dy) * type);
    }

    /// Evaluate the identity (score) of the board state.
    /// Positive scores indicate an advantage for white and
    /// Negative scores indicate an advantage for black.
    int Evaluator::evaluate(Board const& board,
                            unsigned filter /* = material | center | mobility */) {
        int score = 0;

        int mobilityBonus = 3;
        int centerBonus = 5;

        for (unsigned int ndx = 0; ndx < BOARD_SIZE; ndx++) {
            Piece p = board.board[ndx];
            int sideFactor = (getSide(p) == Black) ? -1 : 1;

            /// The score or 'identity property' of the board includes points for
            /// all pieces the player has remaining.
            score += (filter & material) ? sideFactor * materialEvaluator(p) : 0;

            /// The score or 'identity property' of the board includes points for
            /// how close the remaining pieces are to the center of the board.
            score += (filter & center) ? sideFactor * centerEvaluator(ndx, p) * centerBonus : 0;
        }

        /// The score or 'identity property' of the board includes extra points for
        /// how many totals moves (mobility) the remaining pieces can make.
        if (filter & mobility) {
            score += static_cast<int>(board.moves1.size()) * mobilityBonus;
            score -= static_cast<int>(board.moves2.size()) * mobilityBonus;
        }

        //        score *= (board.turn == Black) ? -1 : 1;

        return score;
    }
}  // namespace chess
