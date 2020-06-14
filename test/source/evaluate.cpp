#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#    include <sstream>
#endif

#include <board.h>
#include <chessutil.h>
#include <evaluator.h>

#include <string>

namespace chess {
    using std::cout;
    using std::endl;
    using std::string;

    /**
     * unit tests for Evaluator class
     *
     */
    void show(Board& board) {
        cout << endl;
        cout << "Turn: " << board.turns << " ";
        cout << board.lastMove().to_string() << endl;
        vector<string> lines = Board::to_string(board);
        for (auto& line : lines) {
            cout << line << endl;
        }
        cout << endl;
    }

    TEST_CASE("chess::Evaluate") {
        // Ensure that evaluation is < 0 for black advantage, > 0 for white, == 0 when even.
        Board board;

        board.board.fill(Empty);
        board.board[3 + 3 * 8] = makeSpot(Pawn, White);
        auto score = Evaluator::evaluate(board);
        CHECK(score > 0);

        board.board.fill(Empty);
        board.board[3 + 3 * 8] = makeSpot(Pawn, Black);
        score = Evaluator::evaluate(board);
        CHECK(score < 0);

        board.board.fill(Empty);
        board.board[3 + 3 * 8] = makeSpot(Pawn, White);
        board.board[4 + 4 * 8] = makeSpot(Pawn, Black);
        score = Evaluator::evaluate(board);
        CHECK(score == 0);
    }

}  // namespace chess
