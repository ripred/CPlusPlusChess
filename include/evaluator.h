//
// evaluator.h
//

#pragma once

#include <board.h>
#include <chessutil.h>

namespace chess {
    class Evaluator {
    private:
        // material values
        static int materialEvaluator(Piece p);

        // center location bonus
        static int centerEvaluator(unsigned int location, Piece piece);

    public:
        // Filters to decide what to include in our evaluation
        static unsigned int const material = 0x01u;
        static unsigned int const center = 0x02u;
        static unsigned int const mobility = 0x04u;

        static int evaluate(Board const& board, unsigned int filter = material | center | mobility);
    };
}  // namespace chess
