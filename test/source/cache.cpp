#include <doctest/doctest.h>

#if defined(_WIN32) || defined(WIN32)
// apparently this is required to compile in MSVC++
#    include <sstream>
#endif

#include <board.h>
#include <minimax.h>

namespace chess {
    /**
     * unit tests for MoveCache class
     *
     */
    TEST_CASE("chess::MoveCache") {
        Board game;
        Minimax agent(1);

        agent.useThreads = false;
        agent.useCache = false;
        agent.timeout = 10;
        game.turn = White;

        game.generateMoveLists();
        CHECK(game.board[0] == Rook);
    }
}  // namespace chess