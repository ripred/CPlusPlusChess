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

        agent.useThreads = true;
        agent.useCache = true;
        agent.timeout = 10;
        agent.maxDepth = 2;
        game.turn = White;

        CHECK(agent.cache.cache.empty());
        game.generateMoveLists();
        Move best = agent.bestMove(game);
        CHECK(best.isValid(game));
        CHECK(!agent.cache.cache.empty());
        Entry entry = agent.cache.lookup(game);
        CHECK(entry.isValid(game));
        CHECK(entry.move == best);

        entry.setValue(42);
        CHECK(entry.getValue() == 42);
    }
}  // namespace chess